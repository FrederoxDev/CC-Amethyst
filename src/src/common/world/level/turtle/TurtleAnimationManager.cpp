#include "TurtleAnimationManager.hpp"

std::unordered_map<BlockPos, TurtleMoveAnimation> TurtleAnimationManager::mTurtleMovementAnimations{};

TurtleMoveAnimation::TurtleMoveAnimation()
	: mTurtleStartPos(0, 0, 0), mTurtleEndPos(0, 0, 0), mStartTimestamp(0)
{
	
}

TurtleMoveAnimation::TurtleMoveAnimation(TurtleMovePacket& packet)
	: mTurtleStartPos(packet.mTurtlePosBefore), mTurtleEndPos(packet.mTurtlePosTo), mStartTimestamp(packet.mStartTimestamp)
{

}

void TurtleAnimationManager::OnTurtleMovePacket(TurtleMovePacket& packet)
{
	mTurtleMovementAnimations[packet.mTurtlePosTo] = TurtleMoveAnimation(packet);
}

std::optional<TurtleMoveAnimation> TurtleAnimationManager::TryConsumeAtPos(const BlockPos& position)
{
	auto it = mTurtleMovementAnimations.find(position);
	if (it == mTurtleMovementAnimations.end()) return std::nullopt;

	TurtleMoveAnimation moveAnimation = it->second;
	mTurtleMovementAnimations.erase(it);

	return moveAnimation;
}
