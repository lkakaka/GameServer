
#include "LuaPlugin.h"
#include "Logger.h"
#include "../Common/ServerMacros.h"
#include "LuaTimer.h"
#include "LuaService.h"
#include "LuaScene.h"
#include "LuaRedis.h"
#include "LuaDB.h"
#include "LuaConfig.h"

//#ifndef WIN32
INIT_SINGLETON_CLASS(LuaPlugin)
//#endif

LuaPlugin::LuaPlugin() {
	m_lua.reset(new sol::state());
}

LuaPlugin::~LuaPlugin() {
	//if (m_lua != NULL) lua_close(m_lua);                //¹Ø±Õ»·¾³
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


sol::table LuaPlugin::initLua(const char* funcName) {
	m_lua->open_libraries(sol::lib::base, sol::lib::package, sol::lib::debug, sol::lib::string, sol::lib::table, sol::lib::os);
	initLoggerModule(m_lua);
	LuaTimer::bindLuaTimer(m_lua);
	LuaService::bindLuaService(m_lua);
	LuaScene::bindLuaScene(m_lua);
	LuaRedis::bindLuaRedis(m_lua);
	LuaDB::bindLuaDB(m_lua);
	LuaConfig::bindLuaConfig(m_lua);

	m_lua->script("package.path = '../script/lua/?.lua;'..package.path");
	m_lua->script("package.cpath = '../bin/?.so;'..package.cpath");
	//m_lua->script("package.path = '../script/lua/?.lua;'..package.path");
	m_lua->script("print(package.path)");
	m_lua->script("print(_VERSION)");
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
	return tbl;
}

LuaPlugin* LuaPlugin::getLuaPlugin() {
	return LuaPlugin::getSingleton();
}
