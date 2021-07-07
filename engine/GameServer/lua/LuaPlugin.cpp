
#include "LuaPlugin.h"
#include "Logger.h"

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

void initLoggerModule(std::shared_ptr<sol::state> lua) {
	sol::table logger = lua->create_named_table("Logger");
	logger["log_info"] = &Logger::logInfo;
	logger["log_error"] = &Logger::logError;
	logger["log_warn"] = &Logger::logWarning;
	logger["log_debug"] = &Logger::logDebug;
}

void LuaPlugin::initLua() {
	////1.创建一个state
	//m_lua = luaL_newstate();
	//luaL_openlibs(m_lua);            //打开lua标准库
	////lua_register(L, "foo", foo); //注册c函数到lua环境
	//luaL_dofile(m_lua, "../script/lua/main.lua");     //执行lua脚本

	//sol::state lua;
	m_lua->open_libraries(sol::lib::base, sol::lib::package, sol::lib::debug);
	/*int x = 0;
	lua.set_function("beep", [&x] { ++x; });
	lua.script("beep()");
	assert(x == 1);*/

	initLoggerModule(m_lua);

	m_lua->script("package.path = '../script/lua/?.lua;'..package.path print(package.path)");
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
	callLuaFunc("service_factory", "create_login_service");
}

bool LuaPlugin::callLuaFunc(const char* modName, const char* funcName) {
	sol::function func;
	if (modName == NULL) {
		func = m_lua->get<sol::function>(funcName);
	} else {
		sol::table mod = m_lua->get<sol::table>(modName);
		if (!mod.valid()) {
			Logger::logError("$lua module not exist: %s", modName);
			return false;
		}
		func = mod.get<sol::function>(funcName);
	}
	//if (!func.valid()) return sol::protected_function_result();
	sol::protected_function_result result = func();
	if (!result.valid()) {
		Logger::logError("$lua result = %d", result.status());
		for (auto iter = result.begin(); iter != result.end(); iter++) {
			std::string s = iter->operator std::string();
			Logger::logError("$%s", s.c_str());
		}
		return false;
	}
	return true;
}

LuaPlugin* LuaPlugin::getLuaPlugin() {
	return LuaPlugin::getSingleton();
}