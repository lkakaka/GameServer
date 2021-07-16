#pragma once

#include "sol/sol.hpp"
#include "Redis.h"

USING_DATA_BASE;

class LuaRedis {
private:
	Redis* m_redis;
public:
	LuaRedis(const char* ip, int port);
	static void bindLuaRedis(std::shared_ptr<sol::state> lua);
	sol::object execRedisCmd(const char* cmd, sol::this_state s);
};
