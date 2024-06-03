#pragma once 
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

#include <minecraft/src/common/world/level/block/actor/BlockActor.hpp>
#include <minecraft/src/common/world/level/Level.hpp>
#include <minecraft/src/common/world/level/block/BlockLegacy.hpp>

#include "luaInclude.hpp"
#include "lua/Turtle.hpp"

class TurtleBlockActor : public BlockActor {
public:
	lua_State* L;
	std::mutex luaMutex;
	std::condition_variable cv;
	std::thread luaThread;
	std::atomic<bool> waitingForMain{ false };
	std::chrono::steady_clock::time_point waitStartTime;
	int returnCount = 0;
	TurtleTask task;

public:
	TurtleBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id)
		: BlockActor(type, pos, id), L(nullptr), task(TurtleTask::Undefined)
	{
		// Todo: Setup custom renderer for the block
		mRendererId = BlockActorRendererId::TR_DEFAULT_RENDERER;
		Log::Info("TurtleBlockActor::ctor");
	};

	virtual ~TurtleBlockActor() override {
		// Shutdown the instance of lua on this turtle.
		if (L != nullptr) {
			lua_close(L);
		}
		if (luaThread.joinable()) {
			luaThread.join();
		}

		BlockActor::~BlockActor();
	}

	virtual void tick(BlockSource& region) override {
		if (region.mLevel->isClientSide) return;

		std::lock_guard<std::mutex> lock(luaMutex);
		if (!waitingForMain) return;

		auto currentTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - waitStartTime);
		
		if (task == TurtleTask::InspectDown) {
			const Block& block = region.getBlock(mPosition.below());
			const std::string& name = block.mLegacyBlock->mNameInfo.mFullName.getString();

			lua_pushstring(L, name.c_str());
			returnCount = 1;
			waitingForMain = false;
			cv.notify_one();
			return;
		}

		if (task == TurtleTask::Up && duration >= std::chrono::milliseconds(500)) {
			
			Log::Info("Uppies");
			BlockPos startPos = mPosition;
			moveTo(mPosition.above());

			// then move the block itself?
			const Block& turtleBlock = region.getBlock(startPos);
			region.setBlock(startPos.above(), turtleBlock, 3, nullptr, nullptr);

			//// Send a block change packet
			//GameEvent* blockChangeEvent = (GameEvent*)SlideAddress(0x5665208);
			//region.postGameEvent(nullptr, *blockChangeEvent, startPos.above(), &turtleBlock);

			//const Block& airBlock = region.getBlock(125, 64, -100);
			//region.setBlock(startPos, airBlock, 3, nullptr, nullptr);
			//region.postGameEvent(nullptr, *blockChangeEvent, startPos, &airBlock);
			
			returnCount = 0;
			waitingForMain = false;
			cv.notify_one();
			return;
		}
	}

	void initializeLua() {
		L = luaL_newstate();
		luaL_openlibs(L);

		RegisterTurtleFunctions(L);

		// store a pointer to this block actor so it can be accessed in function impls
		lua_pushlightuserdata(L, this);
		lua_setfield(L, LUA_REGISTRYINDEX, "block_actor");
	}

	void onBlockClicked(BlockSource& region) {
		// Only run lua on the server side.
		if (region.mLevel->isClientSide) return;

		// Stick everything needed in the mutex lock in its own scope
		{
			std::lock_guard<std::mutex> lock(luaMutex);

			if (luaThread.joinable()) {
				Log::Info("Currently running, ignoring click!");
				return;
			}

			if (L == nullptr) initializeLua();

			luaThread = std::thread(&TurtleBlockActor::runLua, this, R"(
				if turtle.inspectDown() == "minecraft:grass_block" then
					turtle.up()
					turtle.up()
				else
					turtle.up()
				end
			)");
		}
	}

	void runLua(const std::string& code)
	{
		if (luaL_dostring(L, code.c_str()) != LUA_OK) {
			std::cerr << "Error running Lua code: " << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
		}
	}

	static TurtleBlockActor& GetBlockActor(lua_State* L) {
		lua_getfield(L, LUA_REGISTRYINDEX, "block_actor");
		TurtleBlockActor* actor = static_cast<TurtleBlockActor*>(lua_touserdata(L, -1));
		lua_pop(L, 1);

		if (actor) return *actor;
		Assert("Failed to find field block_actor with a ptr to TurtleBlockActor");
	}

	static BlockSource& GetBlockSource(lua_State* L) {
		lua_getfield(L, LUA_REGISTRYINDEX, "region");
		BlockSource* region = static_cast<BlockSource*>(lua_touserdata(L, -1));
		lua_pop(L, 1);

		if (region) return *region;
		Assert("Failed to find field region with a ptr to BlockSource");
	}
};