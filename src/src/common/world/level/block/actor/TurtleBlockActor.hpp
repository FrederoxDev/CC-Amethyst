#pragma once 
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

#include <minecraft/src/common/world/level/block/actor/BlockActor.hpp>
#include <minecraft/src/common/world/level/Level.hpp>
#include <minecraft/src/common/world/level/block/BlockLegacy.hpp>

#include <src/common/world/level/computer/LuaInstanceManager.hpp>
#include "src-deps/lua_Include.hpp"

class TurtleBlockActor : public BlockActor {
public:
	LuaInstance* mLuaInstance;

	TurtleBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id)
		: BlockActor(type, pos, id)
	{
		// Todo: Setup custom renderer for the block
		mRendererId = BlockActorRendererId::TR_DEFAULT_RENDERER;
		mLuaInstance = LuaInstanceManager::GetOrCreateInstanceAt(pos);
	};

	virtual void tick(BlockSource& region) override;
};