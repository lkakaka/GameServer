
#include "LuaPlugin.h"
#include "Logger.h"
#include "../Common/ServerMacros.h"
#include "LuaTimer.h"

//#ifndef WIN32
INIT_SINGLETON_CLASS(LuaPlugin)
//#endif

LuaPlugin::LuaPlugin() {
	m_lua.reset(new sol::state());
}

LuaPlugin::~LuaPlugin() {
	//if (m_lua != NULL) lua_close(m_lua);                //关闭环境
	//m_lua = NULL;
}

void test(const char* a) {
	printf("a = %s\n", a);
}

int panicFunc (lua_State* L) {
	size_t messagesize;
	const char* message = lua_tolstring(L, -1, &messagesize);
	if (message) {
		std::string err(message, messagesize);
		Logger::logError("$%s", err.c_str());
	}
	lua_settop(L, 0);
	return -1;
}


int exceptionHandler(lua_State*, sol::optional<const std::exception&> e, sol::string_view v) {
	printf("exceptionHandler");
	return 1;
}

static void initLoggerModule(std::shared_ptr<sol::state> lua) {
	sol::table logger = lua->create_named_table("Logger");
	logger["log_info"] = &Logger::logInfo;
	logger["log_error"] = &Logger::logError;
	logger["log_warn"] = &Logger::logWarning;
	logger["log_debug"] = &Logger::logDebug;
}


//static 
//
//static void initTimerModule(std::shared_ptr<sol::state> lua) {
//	sol::table logger = lua->create_named_table("Timer");
//	logger["log_info"] = &Timer::logInfo;
//	logger["log_error"] = &Logger::logError;
//	logger["log_warn"] = &Logger::logWarning;
//	logger["log_debug"] = &Logger::logDebug;
//}

sol::table LuaPlugin::initLua(const char* funcName) {
	////1.创建一个state
	//m_lua = luaL_newstate();
	//luaL_openlibs(m_lua);            //打开lua标准库
	////lua_register(L, "foo", foo); //注册c函数到lua环境
	//luaL_dofile(m_lua, "../script/lua/main.lua");     //执行lua脚本

	m_lua->open_libraries(sol::lib::base, sol::lib::package, sol::lib::debug, sol::lib::string);
	initLoggerModule(m_lua);
	LuaTimer::bindLuaTimer(m_lua);

	m_lua->script("package.path = '../script/lua/?.lua;'..package.path");
	m_lua->script("print(package.path)");
	//m_lua->set_panic(panicFunc);
	//m_lua->set_exception_handler(exceptionHandler);

	/*sol::protected_function_result result = m_lua->script_file("main.lua");
	if (!result.valid()) {
		Logger::logError("$lua result = %d", result.status());
		for (auto iter = result.begin(); iter != result.end(); iter++) {
			std::string s = iter->operator std::string();
			Logger::logError("$%s", s.c_str());
		}
	}*/
	sol::object obj = m_lua->require_file("main", "../script/lua/main.lua");
	sol::protected_function_result result = callLuaFunc("service_factory", funcName);
	if (!result.valid()) THROW_EXCEPTION("create service error");
	sol::table tbl = result.get<sol::table>(0);
	/*sol::function func = tbl.get<sol::function>("on_recv_service_msg");
	func(tbl, "11", 1, "msg123");
	for (auto iter = tbl.begin(); iter != tbl.end(); iter++) {

	}*/
	return tbl;
}

sol::protected_function_result LuaPlugin::callLuaFunc(const char* modName, const char* funcName) {
	sol::function func;
	if (modName == NULL) {
		func = m_lua->get<sol::function>(funcName);
	} else {
		sol::table mod = m_lua->get<sol::table>(modName);
		if (!mod.valid()) {
			Logger::logError("$lua module not exist: %s", modName);
			return sol::protected_function_result(NULL, -1, 0, 0, sol::call_status::runtime);
		}
		func = mod.get<sol::function>(funcName);
	}
	if (!func.valid()) {
		Logger::logError("$call lua func %s.%s invalid", modName == NULL ? "" : modName, funcName);
		return sol::protected_function_result(NULL, -1, 0, 0, sol::call_status::runtime);
	}
	sol::protected_function_result result = func();
	if (!result.valid()) {
		Logger::logError("$lua result = %d", result.status());
		for (auto iter = result.begin(); iter != result.end(); iter++) {
			std::string s = iter->operator std::string();
			Logger::logError("$%s", s.c_str());
		}
	}
	return result;
}

LuaPlugin* LuaPlugin::getLuaPlugin() {
	return LuaPlugin::getSingleton();
}