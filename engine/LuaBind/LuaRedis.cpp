#include "LuaRedis.h"
#include "Logger.h"

USING_DATA_BASE;

LuaRedis::LuaRedis(const char* ip, int port) {
	std::string _ip = ip;
	m_redis = new Redis(ip, port);
}

void LuaRedis::bindLuaRedis(std::shared_ptr<sol::state> lua) {
	sol::usertype<LuaRedis> luaRedis_type = lua->new_usertype<LuaRedis>("LuaRedis",
		// 3 constructors
		sol::constructors<LuaRedis(const char*, int)>());

	// typical member function that returns a variable
	luaRedis_type["execRedisCmd"] = &LuaRedis::execRedisCmd;

}

static sol::object parseRedisReply(redisReply* reply, sol::this_state s) {
	sol::state_view lua(s);
	switch (reply->type) {
		case REDIS_REPLY_INTEGER:
			return sol::make_object(lua, reply->integer);
		case REDIS_REPLY_DOUBLE:
			return sol::make_object(lua, reply->dval);
		case REDIS_REPLY_NIL:
			return sol::nil;
		case REDIS_REPLY_BOOL:
			return sol::make_object(lua, reply->integer);
		case REDIS_REPLY_ATTR:
		case REDIS_REPLY_PUSH:
		case REDIS_REPLY_BIGNUM:
			return sol::make_object(lua, reply->str);
			//return PyFloat_FromString(PyUnicode_FromStringAndSize(reply->str, reply->len));
		case REDIS_REPLY_STRING:
		case REDIS_REPLY_ERROR:
		case REDIS_REPLY_STATUS: {
			sol::object obj = sol::make_object(lua, reply->str);
			if (obj == sol::nil) {
				printf("error");
			}
			return obj;
		}
			//return PyUnicode_FromStringAndSize(reply->str, reply->len);
		case REDIS_REPLY_ARRAY:
		case REDIS_REPLY_MAP:
		case REDIS_REPLY_SET:
		{
			//PyObject* array = PyTuple_New(reply->elements);
			sol::table tbl = sol::table::create_with(s.lua_state());
			for (int i = 0; i < reply->elements; i++) {
				redisReply* subReply = reply->element[i];
				tbl[i+1] = parseRedisReply(subReply, s);
				//PyTuple_SetItem(array, i, parseRedisReply(subReply));
			}
			return tbl;
		}
	}
	Logger::logError("$not support redis reply type %d", reply->type);
	return sol::nil;
}


sol::object LuaRedis::execRedisCmd(const char* redisCmd, sol::this_state s) {
	//PROFILE_TRACK("redis_cmd");
	REDIS_REPLY_PTR ptr = m_redis->execRedisCmd(redisCmd);
	if (ptr == NULL) {
		return sol::nil;
	}

	return parseRedisReply(ptr->getReply(), s);
}


