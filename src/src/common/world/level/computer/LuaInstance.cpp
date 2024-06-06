#include "LuaInstance.hpp"
#include "LuaInstanceManager.hpp"

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

int inspectDown(lua_State* L) {
	LuaInstance& luaInstance = LuaInstanceManager::GetInstanceFromLua(L);

	// We need to wait for the main thread in order to continue
	// We don't want to run MC code on a different thread to prevent UB
	{
		std::unique_lock<std::mutex> lock(luaInstance.mLuaMutex);
		luaInstance.mWaitingForMain = true;
		luaInstance.mCv.wait(lock, [&]() { return !luaInstance.mWaitingForMain; });
	}

	// Get the response from the main thread and return it
	int returnCount = luaInstance.mReturnCount;
	luaInstance.mReturnCount = 0;

	return returnCount;
}

void LuaInstance::_InitializeLuaInstance()
{
	std::lock_guard<std::mutex> lock(mLuaMutex);

	mL = luaL_newstate();
	luaL_openlibs(mL);

	// register functions here
	lua_newtable(mL);

	lua_pushcfunction(mL, inspectDown);
	lua_setfield(mL, -2, "inspectDown");

	lua_setglobal(mL, "turtle");

	// Setup ptr to this inside lua instance
	lua_pushlightuserdata(mL, this);
	lua_setfield(mL, LUA_REGISTRYINDEX, "lua_instance");
}

void LuaInstance::_RunLua(const std::string& code)
{
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
