#pragma once
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class LuaPlugin {
public:
	static void initLua();
};
