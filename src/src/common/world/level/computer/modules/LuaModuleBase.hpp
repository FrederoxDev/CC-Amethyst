#pragma once
#include "src-deps/lua_Include.hpp"
#include <src/common/world/level/computer/LuaInstance.hpp>
#include <src/common/world/level/computer/LuaInstanceManager.hpp>

#define LuaFunction(funcName) int _##funcName(lua_State* L) { return cppFunctionBase(L, funcName); }

#define RegisterLuaFunction(L, funcName) lua_pushcfunction(L, _##funcName); lua_setfield(L, -2, #funcName)

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

	if (luaInstance.mLuaSleepTimeMs != 0) {
		Log::Info("turtle thread sleeping for {:d}", luaInstance.mLuaSleepTimeMs);
		std::this_thread::sleep_for(std::chrono::milliseconds(luaInstance.mLuaSleepTimeMs));
		luaInstance.mLuaSleepTimeMs = 0;
	}

	return returnCount;
}

uint64_t getTimestamp() {
	auto epoc = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(epoc).count();
}

class LuaModuleBase {
public:
	virtual void RegisterModule(lua_State* L) = 0;
};