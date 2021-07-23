#pragma once

#include "Config.h"
#include "sol/sol.hpp"

class LuaConfig {
public:
	static void bindLuaConfig(std::shared_ptr<sol::state>);
};
