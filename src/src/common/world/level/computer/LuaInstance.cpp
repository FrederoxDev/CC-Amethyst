#include "LuaInstance.hpp"
#include "LuaInstanceManager.hpp"
#include <minecraft/src/common/network/LoopbackPacketSender.hpp>
#include "src/common/world/level/block/actor/TurtleBlockActor.hpp"
#include <src/common/network/packet/TurtleActionPacket.hpp>

LuaInstance::~LuaInstance()
{
	{
		std::lock_guard<std::mutex> lock(mLuaMutex);

		if (mL != nullptr) {
			lua_close(mL);
		}
	}

	if (mLuaThread.joinable()) {
		mLuaThread.join();
	}
}

LuaInstance::LuaInstance() : mL(nullptr), mIsRunning(false)
{
	Log::Info("New LuaInstance made!");
	_InitializeLuaInstance();
}

void LuaInstance::RunLua(const std::string& code)
{
	std::lock_guard<std::mutex> lock(mLuaMutex);

	if (mIsRunning) {
		Log::Info("Lua code is still running, ignoring new request");
		return;
	}

	// Allow the previous thread to do any destruction work
	if (mLuaThread.joinable()) {
		mLuaThread.join();
	}

	mLuaThread = std::thread(&LuaInstance::_RunLua, this, code);
}

int cppFunctionBase(lua_State* L, LuaInstance::CppCallback callback) {
	LuaInstance& luaInstance = LuaInstanceManager::GetInstanceFromLua(L);

	// We need to wait for the main thread in order to continue
	// We don't want to run MC code on a different thread to prevent UB
	{
		std::unique_lock<std::mutex> lock(luaInstance.mLuaMutex);
		luaInstance.mWaitingForMain = true;
		luaInstance.mInternalCppCallback = callback;
		luaInstance.mCv.wait(lock, [&]() { return !luaInstance.mWaitingForMain; });
	}

	// Get the response from the main thread and return it
	int returnCount = luaInstance.mReturnCount;
	luaInstance.mReturnCount = 0;

	return returnCount;
}

int inspectUp(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	const Block& block = region.getBlock(turtle.mPosition.above());
	lua_pushstring(lua.mL, block.mLegacyBlock->mNameInfo.mFullName.c_str());
	return 1;
}

int _inspectUp(lua_State* L) {
	return cppFunctionBase(L, inspectUp);
}

int inspectDown(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	const Block& block = region.getBlock(turtle.mPosition.below());
	lua_pushstring(lua.mL, block.mLegacyBlock->mNameInfo.mFullName.c_str());
	return 1;
}

int _inspectDown(lua_State* L) {
	return cppFunctionBase(L, inspectDown);
}

int up(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	const Block& turtleBlock = region.getBlock(turtle.mPosition);
	const Block& aboveBlock = region.getBlock(turtle.mPosition.above());
	BlockPos originalPos = turtle.mPosition;
	 
	// check if the block is air
	// it probably makes sense to allow moving through other blocks, but for now this is fine
	if (aboveBlock.mLegacyBlock->mNameInfo.mFullName.getHash() != HashedString::computeHash("minecraft:air")) {
		return 0;
	}

	LuaInstanceManager::MoveInstance(originalPos, originalPos.above());
	region.setBlock(originalPos, aboveBlock, 3, nullptr, nullptr);
	region.setBlock(originalPos.above(), turtleBlock, 3, nullptr, nullptr);

	GameEvent* blockChangeEvent = (GameEvent*)SlideAddress(0x5665208);
	region.postGameEvent(nullptr, *blockChangeEvent, originalPos, &turtleBlock);
	region.postGameEvent(nullptr, *blockChangeEvent, originalPos.above(), &aboveBlock);

	Log::Info("mPacketSender 0x{:x}", *(uintptr_t*)region.mLevel->mPacketSender - GetMinecraftBaseAddress());
	TurtleActionPacket actionPacket = TurtleActionPacket();
	region.mLevel->mPacketSender->sendBroadcast(actionPacket);

	return 0;
}

int _up(lua_State* L) {
	return cppFunctionBase(L, up);
}
 
void LuaInstance::_InitializeLuaInstance()
{
	std::lock_guard<std::mutex> lock(mLuaMutex);

	mL = luaL_newstate();
	luaL_openlibs(mL);

	// register functions here
	lua_newtable(mL);

	lua_pushcfunction(mL, _inspectUp);
	lua_setfield(mL, -2, "inspectUp");

	lua_pushcfunction(mL, _inspectDown);
	lua_setfield(mL, -2, "inspectDown");

	lua_pushcfunction(mL, _up);
	lua_setfield(mL, -2, "up");

	lua_setglobal(mL, "turtle");

	// Setup ptr to this inside lua instance
	lua_pushlightuserdata(mL, this);
	lua_setfield(mL, LUA_REGISTRYINDEX, "lua_instance");
}

void LuaInstance::_RunLua(const std::string& code)
{
	Log::Info("_RunLua");

	{
		std::lock_guard<std::mutex> lock(mLuaMutex);
		mIsRunning = true;
	}

	if (luaL_dostring(mL, code.c_str()) != LUA_OK) {
		std::cerr << "Error running Lua code: " << lua_tostring(mL, -1) << std::endl;
		lua_pop(mL, 1);
	}

	{
		std::lock_guard<std::mutex> lock(mLuaMutex);
		mIsRunning = false;
	}
}
