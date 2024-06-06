#include "LuaInstanceManager.hpp"

std::unordered_map<BlockPos, std::shared_ptr<LuaInstance>> LuaInstanceManager::mWorldPositionToLuaInstance;
std::mutex LuaInstanceManager::mInstanceMapMutex;

LuaInstance* LuaInstanceManager::GetOrCreateInstanceAt(const BlockPos& position)
{
	std::lock_guard<std::mutex> lock(mInstanceMapMutex);

	auto it = mWorldPositionToLuaInstance.find(position);

	if (it != mWorldPositionToLuaInstance.end()) return it->second.get();

	mWorldPositionToLuaInstance[position] = std::make_shared<LuaInstance>();
	return mWorldPositionToLuaInstance[position].get();
}

LuaInstance& LuaInstanceManager::GetInstanceFromLua(lua_State* L)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "lua_instance");
	LuaInstance* instance = static_cast<LuaInstance*>(lua_touserdata(L, -1));
	lua_pop(L, 1);

	if (instance) return *instance;
	Assert("Failed to get field 'lua_instance' from lua_State*");
}
