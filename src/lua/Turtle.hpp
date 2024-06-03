#pragma once
#include <amethyst/Log.hpp>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

enum TurtleTask {
	Undefined,
	InspectDown,
	Up
};

namespace TurtleGlobal {
	int Up(lua_State* L);
	int InspectDown(lua_State* L);
};

void RegisterTurtleFunctions(lua_State* L);