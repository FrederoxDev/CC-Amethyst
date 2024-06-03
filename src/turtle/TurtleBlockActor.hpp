#pragma once 
#include <minecraft/src/common/world/level/block/actor/BlockActor.hpp>
#include <minecraft/src/common/world/level/Level.hpp>

#include "luaInclude.hpp"
#include "lua/Turtle.hpp"

class TurtleBlockActor : public BlockActor {
public:
	lua_State* L;

	TurtleBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id)
		: BlockActor(type, pos, id)
	{
		// Todo: Setup custom renderer for the block
		mRendererId = BlockActorRendererId::TR_DEFAULT_RENDERER;
		L = nullptr;
	};

	virtual ~TurtleBlockActor() override {
		// Shutdown the instance of lua on this turtle.
		if (L != nullptr) {
			lua_close(L);
		}

		BlockActor::~BlockActor();
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
		if (region.mLevel->isClientSide) return;

		if (L == nullptr) initializeLua();

		// Set a ptr to BlockSource
		lua_pushlightuserdata(L, &region);
		lua_setfield(L, LUA_REGISTRYINDEX, "region");

		std::string luaScript = R"(
			for i = 0, 3 do
				turtle.up()
			end
		)";

		if (luaL_dostring(L, luaScript.c_str()) != LUA_OK) {
			std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
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