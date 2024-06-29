#pragma	once
#include <unordered_map>
#include <memory>
#include <minecraft/src/common/world/level/BlockPos.hpp>
#include "src/common/network/packet/TurtleMovePacket.hpp"
#include <src/common/network/packet/TurtleRotatePacket.hpp>

class TurtleMoveAnimation {
public:
	BlockPos mTurtleStartPos;
	BlockPos mTurtleEndPos;
	std::chrono::milliseconds mStartTimestamp;

public:
	TurtleMoveAnimation();
	TurtleMoveAnimation(TurtleMovePacket& packet);
	TurtleMoveAnimation& operator=(const TurtleMoveAnimation& other) = default;
};

class TurtleRotateAnimation {
public:
	BlockPos mTurtlePos;
	FacingID mOldDir;
	FacingID mNewDir;
	std::chrono::milliseconds mTimestamp;

public:
	TurtleRotateAnimation(TurtleRotatePacket& packet);
	TurtleRotateAnimation& operator=(const TurtleRotateAnimation& other) = default;
};

class TurtleAnimationManager {
private:
	static std::unordered_map<BlockPos, TurtleMoveAnimation> mTurtleMovementAnimations;

public:
	static void OnTurtleMovePacket(TurtleMovePacket& packet);
	static std::optional<TurtleMoveAnimation> TryConsumeMovementPacket(const BlockPos& position);
};