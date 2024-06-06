#include "LuaInstanceManager.hpp"

std::unordered_map<BlockPos, std::shared_ptr<LuaInstance>> LuaInstanceManager::mWorldPositionToLuaInstance;
std::mutex LuaInstanceManager::mInstanceMapMutex;

bool LuaInstanceManager::IsInstanceAt(const BlockPos& position)
{
	auto it = mWorldPositionToLuaInstance.find(position);
	return it != mWorldPositionToLuaInstance.end();
}

LuaInstance* LuaInstanceManager::GetOrCreateInstanceAt(const BlockPos& position)
{
	std::lock_guard<std::mutex> lock(mInstanceMapMutex);

	auto it = mWorldPositionToLuaInstance.find(position);

	if (it != mWorldPositionToLuaInstance.end()) return it->second.get();

	Log::Info("Making new instance at {}", position);
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

void LuaInstanceManager::MoveInstance(const BlockPos& from, const BlockPos& to)
{
	std::lock_guard<std::mutex> lock(mInstanceMapMutex);

	auto it = mWorldPositionToLuaInstance.find(from);
	if (it == mWorldPositionToLuaInstance.end()) {
		Assert("Tried to move a LuaInstance from {}, but no instance exists at this position.", from);
	}

	auto newIt = mWorldPositionToLuaInstance.find(to);
	if (newIt != mWorldPositionToLuaInstance.end()) {
		Assert("Tried to move a LuaInstance from {} to {} but an instance already exists at the new location", to, from);
	}

	mWorldPositionToLuaInstance[to] = std::move(it->second);
	mWorldPositionToLuaInstance.erase(it);
}
