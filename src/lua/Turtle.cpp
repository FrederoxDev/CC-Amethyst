#include "Turtle.hpp"
#include "turtle/TurtleBlockActor.hpp"
#include <minecraft/src/common/world/level/block/BlockLegacy.hpp>
#include <minecraft/src/common/world/level/block/registry/BlockTypeRegistry.hpp>

int TurtleGlobal::Up(lua_State* L)
{
	TurtleBlockActor& actor = TurtleBlockActor::GetBlockActor(L);
	BlockSource& region = TurtleBlockActor::GetBlockSource(L);

	// Move the block actor to the new position
	BlockPos startPos = actor.mPosition;
	actor.moveTo(actor.mPosition.above());

	// then move the block itself?
	const Block& turtleBlock = region.getBlock(startPos);
	region.setBlock(startPos.above(), turtleBlock, 3, nullptr, nullptr);

	// Send a block change packet
	GameEvent* blockChangeEvent = (GameEvent*)SlideAddress(0x5665208);
	region.postGameEvent(nullptr, *blockChangeEvent, startPos.above(), &turtleBlock);

	const Block & airBlock = region.getBlock(6, 86, 86);
	region.setBlock(startPos, airBlock, 3, nullptr, nullptr);
	region.postGameEvent(nullptr, *blockChangeEvent, startPos, &airBlock);

	Log::Info("{}", actor.mPosition);

	return 0;
}

int TurtleGlobal::InspectDown(lua_State* L)
{
	TurtleBlockActor& actor = TurtleBlockActor::GetBlockActor(L);
	BlockSource& region = TurtleBlockActor::GetBlockSource(L);

	const Block& block = region.getBlock(actor.mPosition.below());
	const std::string& name = block.mLegacyBlock->mNameInfo.mFullName.getString();

	lua_pushstring(L, name.c_str());
	return 1;
}

void RegisterTurtleFunctions(lua_State* L)
{
	lua_newtable(L);

	lua_pushcfunction(L, TurtleGlobal::Up);
	lua_setfield(L, -2, "up");

	lua_pushcfunction(L, TurtleGlobal::InspectDown);
	lua_setfield(L, -2, "inspectDown");

	lua_setglobal(L, "turtle");
}