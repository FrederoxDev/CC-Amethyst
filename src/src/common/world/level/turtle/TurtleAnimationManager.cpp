#include "TurtleAnimationManager.hpp"
#include <src/common/network/packet/TurtleRotatePacket.hpp>

std::unordered_map<BlockPos, TurtleMoveAnimation> TurtleAnimationManager::mTurtleMovementAnimations{};

TurtleMoveAnimation::TurtleMoveAnimation()
	: mTurtleStartPos(0, 0, 0), mTurtleEndPos(0, 0, 0), mStartTimestamp(0)
{
	
}

TurtleMoveAnimation::TurtleMoveAnimation(TurtleMovePacket& packet)
	: mTurtleStartPos(packet.mTurtlePosBefore), mTurtleEndPos(packet.mTurtlePosTo), mStartTimestamp(packet.mTimestamp)
{

}

void TurtleAnimationManager::OnTurtleMovePacket(TurtleMovePacket& packet)
{
	mTurtleMovementAnimations[packet.mTurtlePosTo] = TurtleMoveAnimation(packet);
}

std::optional<TurtleMoveAnimation> TurtleAnimationManager::TryConsumeMovementPacket(const BlockPos& position)
{
	auto it = mTurtleMovementAnimations.find(position);
	if (it == mTurtleMovementAnimations.end()) return std::nullopt;

	TurtleMoveAnimation moveAnimation = it->second;
	mTurtleMovementAnimations.erase(it);

	return moveAnimation;
}

TurtleRotateAnimation::TurtleRotateAnimation(TurtleRotatePacket& packet)
	: mTurtlePos(packet.mTurtlePos), mOldDir(packet.mOldDir), mNewDir(packet.mNewDir), mTimestamp(packet.mTimestamp)
{
}
