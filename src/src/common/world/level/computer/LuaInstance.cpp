#include "LuaInstance.hpp"
#include "LuaInstanceManager.hpp"
#include <minecraft/src/common/network/LoopbackPacketSender.hpp>
#include "src/common/world/level/block/actor/TurtleBlockActor.hpp"
#include "src/common/network/packet/TurtleMovePacket.hpp"
#include "src/common/world/level/computer/modules/TurtleModule.hpp"

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

 
void LuaInstance::_InitializeLuaInstance()
{
	std::lock_guard<std::mutex> lock(mLuaMutex);

	mL = luaL_newstate();
	luaL_openlibs(mL);

	TurtleModule().RegisterModule(mL);

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
