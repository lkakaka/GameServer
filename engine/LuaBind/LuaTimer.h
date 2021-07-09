#pragma once

#include "Timer.h"
#include "sol/sol.hpp"

class LuaTimer {
public:
	static void bindLuaTimer(std::shared_ptr<sol::state>);
	static long addTimer(int firstInterval, int interval, int loopCnt, sol::function func);
	static void removeTimer(int iTimerId);
	
};
