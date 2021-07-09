#include "LuaTimer.h"

void LuaTimer::bindLuaTimer(std::shared_ptr<sol::state> lua) {
	sol::table timer = lua->create_named_table("Timer");
	timer["addTimer"] = &LuaTimer::addTimer;
	timer["removeTimer"] = &LuaTimer::removeTimer;
}

long LuaTimer::addTimer(int firstInterval, int interval, int loopCnt, sol::function func) {
	return TimerMgr::getSingleton()->addTimer(firstInterval, interval, loopCnt, [func](int timerId) { func(timerId); });
}

void LuaTimer::removeTimer(int timerId) {
	TimerMgr::getSingleton()->removeTimer(timerId, true);
}
