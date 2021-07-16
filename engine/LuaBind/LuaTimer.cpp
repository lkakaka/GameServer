#include "LuaTimer.h"
#include "Logger.h"

void LuaTimer::bindLuaTimer(std::shared_ptr<sol::state> lua) {
	sol::table timer = lua->create_named_table("Timer");
	timer["addTimer"] = &LuaTimer::addTimer;
	timer["removeTimer"] = &LuaTimer::removeTimer;
}

long LuaTimer::addTimer(int firstInterval, int interval, int loopCnt, sol::function func) {
	return TimerMgr::getSingleton()->addTimer(firstInterval, interval, loopCnt, [func](int timerId) { 
			sol::protected_function_result result =	func(timerId);
			if (!result.valid()) {
				Logger::logError("$lua result = %d", result.status());
				sol::error err = result;
				std::string what = err.what();
				std::cout << what << std::endl;
				Logger::logError("$%s", err.what());
			}
		});
}

void LuaTimer::removeTimer(int timerId) {
	TimerMgr::getSingleton()->removeTimer(timerId, true);
}
