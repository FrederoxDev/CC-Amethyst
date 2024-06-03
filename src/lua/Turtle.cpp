#include "Turtle.hpp"
#include "turtle/TurtleBlockActor.hpp"
#include <minecraft/src/common/world/level/block/BlockLegacy.hpp>

int TurtleGlobal::Forward(lua_State* L)
{
	TurtleBlockActor& actor = TurtleBlockActor::GetBlockActor(L);
	BlockSource& region = TurtleBlockActor::GetBlockSource(L);
	Log::Info("{}", actor.mPosition);

	Log::Info("{:d}", region.mMinHeight);

	return 0;
}

int TurtleGlobal::Back(lua_State* L)
{
	Log::Info("Turtle::Back");
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

	lua_pushcfunction(L, TurtleGlobal::Forward);
	lua_setfield(L, -2, "forward");

	lua_pushcfunction(L, TurtleGlobal::Back);
	lua_setfield(L, -2, "back");

	lua_pushcfunction(L, TurtleGlobal::InspectDown);
	lua_setfield(L, -2, "inspectDown");

	lua_setglobal(L, "turtle");
}