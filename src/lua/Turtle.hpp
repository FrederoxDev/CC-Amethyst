#pragma once
#include <amethyst/Log.hpp>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace TurtleGlobal {
	int Forward(lua_State* L);
	int Back(lua_State* L);
	int InspectDown(lua_State* L);
};

void RegisterTurtleFunctions(lua_State* L);