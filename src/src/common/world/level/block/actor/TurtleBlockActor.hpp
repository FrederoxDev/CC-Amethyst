#pragma once 
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <optional>

#include <minecraft/src/common/world/level/block/actor/BlockActor.hpp>
#include <minecraft/src/common/world/level/Level.hpp>
#include <minecraft/src/common/world/level/block/BlockLegacy.hpp>

#include <src/common/world/level/computer/LuaInstanceManager.hpp>
#include "src/common/world/level/turtle/TurtleAnimationManager.hpp"
#include "src-deps/lua_Include.hpp"

static BlockActorRendererId turtleBlockActorRendererId;

class TurtleBlockActor : public BlockActor {
public:
	LuaInstance* mLuaInstance;
	std::optional<TurtleMoveAnimation> mMoveAnimation;

	TurtleBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id)
		: BlockActor(type, pos, id)
	{
		mRendererId = (BlockActorRendererId)26;
		mLuaInstance = nullptr;
	};

	virtual void onPlace(BlockSource& region) override;
	virtual void tick(BlockSource& region) override;
};