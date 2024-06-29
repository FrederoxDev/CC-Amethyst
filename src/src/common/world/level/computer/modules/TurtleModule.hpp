#pragma once
#include "src/common/world/level/computer/modules/LuaModuleBase.hpp"
#include "src/common/world/level/block/actor/TurtleBlockActor.hpp"
#include <minecraft/src/common/network/LoopbackPacketSender.hpp>
#include <src/common/network/packet/TurtleRotatePacket.hpp>

int inspectUp(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	const Block& block = region.getBlock(turtle.mPosition.above());
	lua_pushstring(lua.mL, block.mLegacyBlock->mNameInfo.mFullName.c_str());
	return 1;
}

int inspectDown(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	const Block& block = region.getBlock(turtle.mPosition.below());
	lua_pushstring(lua.mL, block.mLegacyBlock->mNameInfo.mFullName.c_str());
	return 1;
}

inline int arbitraryTryMove(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region, const BlockPos& from, const BlockPos& to) {
	const Block& turtleBlock = region.getBlock(from);
	const Block& targetBlock = region.getBlock(to);

	// Check if its a valid block to move into
	if (targetBlock.mLegacyBlock->mNameInfo.mFullName.getHash() != HashedString::computeHash("minecraft:air")) {
		return 0;
	}

	// Move the physical turtle block to the new pos
	LuaInstanceManager::MoveInstance(from, to);
	region.removeBlock(from);
	region.setBlock(to, turtleBlock, 3, nullptr, nullptr);

	// Create movement update packet
	TurtleMovePacket movementPacket = TurtleMovePacket();
	movementPacket.mTimestamp = getTimestamp();
	movementPacket.mTurtlePosBefore = from;
	movementPacket.mTurtlePosTo = to;

	// Notify all clients that this movement has happened
	region.mLevel->mPacketSender->sendBroadcast(movementPacket);

	// Wait for the movement animation to play
	lua.mLuaSleepTimeMs = 600;
	return 0;
}

int up(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	return arbitraryTryMove(lua, turtle, region, turtle.mPosition, turtle.mPosition.above());
}

int down(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	return arbitraryTryMove(lua, turtle, region, turtle.mPosition, turtle.mPosition.below());
}

int turnLeft(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	Log::Info("turnLeft!");

	// Rotate the turtle on the server-side
	FacingID originalRot = turtle.mTurtleRot;
	FacingID newRot = Facing::getCounterClockWise(originalRot);
	turtle.mTurtleRot = newRot; // <-- this needs to be moved to a persistent storage, destroyed on move!

	// Create rotation update packet
	TurtleRotatePacket rotatePacket;
	rotatePacket.mTimestamp = getTimestamp();
	rotatePacket.mTurtlePos = turtle.mPosition;
	rotatePacket.mOldDir = originalRot;
	rotatePacket.mNewDir = newRot;

	// Notify all clients that this movement has happened
	region.mLevel->mPacketSender->sendBroadcast(rotatePacket);

	// Wait for the rotation animation to play
	lua.mLuaSleepTimeMs = 600;
	return 0;
}

int forward(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	BlockPos targetPos = turtle.mPosition.neighbor(turtle.mTurtleRot);
	return arbitraryTryMove(lua, turtle, region, turtle.mPosition, targetPos);
}

int back(LuaInstance& lua, TurtleBlockActor& turtle, BlockSource& region) {
	BlockPos targetPos = turtle.mPosition.neighbor(Facing::getOpposite(turtle.mTurtleRot));
	return arbitraryTryMove(lua, turtle, region, turtle.mPosition, targetPos);
}

LuaFunction(inspectUp);
LuaFunction(inspectDown);
LuaFunction(up);
LuaFunction(down);
LuaFunction(forward);
LuaFunction(back);
LuaFunction(turnLeft);

class TurtleModule : public LuaModuleBase {
public:
	TurtleModule() {}

	virtual void RegisterModule(lua_State* L) {
		lua_newtable(L);

		RegisterLuaFunction(L, inspectUp);
		RegisterLuaFunction(L, inspectDown);
		RegisterLuaFunction(L, up);
		RegisterLuaFunction(L, down);
		RegisterLuaFunction(L, forward);
		RegisterLuaFunction(L, back);
		RegisterLuaFunction(L, turnLeft);

		lua_setglobal(L, "turtle");
	}
};