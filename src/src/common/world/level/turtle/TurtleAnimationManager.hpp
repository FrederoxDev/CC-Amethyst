#pragma	once
#include <unordered_map>
#include <memory>
#include <minecraft/src/common/world/level/BlockPos.hpp>
#include "src/common/network/packet/TurtleMovePacket.hpp"

class TurtleMoveAnimation {
public:
	BlockPos mTurtleStartPos;
	BlockPos mTurtleEndPos;
	std::chrono::milliseconds mStartTimestamp;

public:
	TurtleMoveAnimation();
	TurtleMoveAnimation(TurtleMovePacket& packet);
};

class TurtleAnimationManager {
private:
	static std::unordered_map<BlockPos, TurtleMoveAnimation> mTurtleMovementAnimations;

public:
	static void OnTurtleMovePacket(TurtleMovePacket& packet);
	static std::optional<TurtleMoveAnimation> TryConsumeAtPos(const BlockPos& position);
};