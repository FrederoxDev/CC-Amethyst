#pragma once
#include <mutex>
#include <amethyst/Log.hpp>
#include <minecraft/src/common/world/level/BlockSource.hpp>
#include "src-deps/lua_Include.hpp"

class TurtleBlockActor;

class LuaInstance {
public:
    using CppCallback = int(*)(LuaInstance&, TurtleBlockActor&, BlockSource&);

public:
    lua_State* mL;
    std::mutex mLuaMutex;
    std::condition_variable mCv;
    std::thread mLuaThread;
    std::atomic<bool> mWaitingForMain{ false };
    CppCallback mInternalCppCallback;
    int mReturnCount = 0;
    bool mIsRunning;

    ~LuaInstance();
    LuaInstance();
    void RunLua(const std::string& code);
    
private:
    void _RunLua(const std::string& code);
    void _InitializeLuaInstance();
};