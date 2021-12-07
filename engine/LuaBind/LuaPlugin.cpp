
#include "LuaPlugin.h"
#include "Logger.h"
#include "../Common/ServerMacros.h"
#include "LuaTimer.h"
#include "LuaService.h"
#include "LuaScene.h"
#include "LuaRedis.h"
#include "LuaDB.h"
#include "LuaConfig.h"
#include "LuaCrypt.h"
#include "LuaHttp.h"
#include "AsioService.h"

//#ifndef WIN32
INIT_SINGLETON_CLASS(LuaPlugin)
//#endif

LuaPlugin::LuaPlugin(const char* entryFuncName) {
	m_lua.reset(new sol::state());
	initLua(entryFuncName);
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
		LOG_ERROR("%s", err.c_str());
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
	// 必须使用LOG_INFO("%s", s.c_str())，如果使用 LOG_INFO(s.c_str()),字符串中含有%有可能会crash
	logger["log_info"] = [](std::string s) { LOG_INFO("%s", s.c_str()); };
	logger["log_error"] = [](std::string s) { LOG_ERROR("%s", s.c_str()); };
	logger["log_warn"] = [](std::string s) { LOG_WARN("%s", s.c_str()); };
	logger["log_debug"] = [](std::string s) { LOG_DEBUG("%s", s.c_str()); };
}

sol::table LuaPlugin::initLua(const char* funcName) {
	m_lua->open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::os, sol::lib::math, 
		sol::lib::table, sol::lib::debug, sol::lib::bit32, sol::lib::io, sol::lib::lfs);
	initLoggerModule(m_lua);
	LuaTimer::bindLuaTimer(m_lua);
	LuaService::bindLuaService(m_lua);
	LuaScene::bindLuaScene(m_lua);
	LuaRedis::bindLuaRedis(m_lua);
	LuaDB::bindLuaDB(m_lua);
	LuaConfig::bindLuaConfig(m_lua);
	LuaCrypt::bindLuaCrypt(m_lua);
	LuaHttp::bindLuaHttp(m_lua);

	m_lua->script("package.path = '../script/lua/?.lua;'..package.path");
	m_lua->script("package.cpath = '../bin/?.so;'..package.cpath");
	//m_lua->script("print(package.path)");
	//m_lua->script("print(_VERSION)");

	//m_lua->set_panic(panicFunc);
	//m_lua->set_exception_handler(exceptionHandler);

	//m_lua->require_file("main", "../script/lua/main.lua");
	m_lua->script_file("../script/lua/main.lua");
	sol::function::set_default_handler(((*m_lua)["got_problems"]));
	sol::protected_function_result result = callLuaFunc("service_factory", funcName);
	if (!result.valid()) THROW_EXCEPTION("create service error");
	m_service = result.get<sol::table>(0);

	return m_service;
}

LuaPlugin* LuaPlugin::getLuaPlugin() {
	return LuaPlugin::getSingleton();
}

void LuaPlugin::initScript() {
	
}

void LuaPlugin::dispatchClientMsgToScript(int connId, int msgId, const char* data, int len) {
	char* buff = new char[len + 1]{ 0 };
	memcpy(buff, data, len);
	sol::function func = m_service.get<sol::function>("on_recv_client_msg");
	LuaPlugin::callLuaFunc(func, m_service, connId, msgId, buff);
	delete[] buff;
}

void LuaPlugin::dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len) {
	char* buff = new char[len + 1]{ 0 };
	memcpy(buff, data, len);
	sol::function func = m_service.get<sol::function>("on_recv_service_msg");
	LuaPlugin::callLuaFunc(func, m_service, srcAddr->getName(), msgId, buff);
	delete[] buff;
}
