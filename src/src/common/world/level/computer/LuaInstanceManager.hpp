#pragma once
#include <unordered_map>
#include <minecraft/src/common/world/level/BlockPos.hpp>
#include <amethyst/Log.hpp>
#include "src/common/world/level/computer/LuaInstance.hpp"

class LuaInstanceManager {
private:
    static std::unordered_map<BlockPos, std::shared_ptr<LuaInstance>> mWorldPositionToLuaInstance;
    static std::mutex mInstanceMapMutex;

public:
    static LuaInstance* GetOrCreateInstanceAt(const BlockPos& position);
    static LuaInstance& GetInstanceFromLua(lua_State* L);
};