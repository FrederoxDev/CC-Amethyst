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
	static BlockActorRendererId TURTLE_RENDERER_ID;

	LuaInstance* mLuaInstance;
	std::variant<std::monostate, TurtleMoveAnimation, TurtleRotateAnimation> mAnimation;

	// No persistent data should be placed here as a member variable
	// Data here is wiped on turtle move
	FacingID mTurtleRot;

	TurtleBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id)
		: BlockActor(type, pos, id), mTurtleRot(FacingID::NORTH)
	{
		mRendererId = TURTLE_RENDERER_ID;
		mLuaInstance = nullptr;
	};

	virtual void onPlace(BlockSource& region) override;
	virtual void tick(BlockSource& region) override;
};