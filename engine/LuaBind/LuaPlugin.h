#pragma once

#define SOL_ALL_SAFETIES_ON 1

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "Singleton.h"
#include "../Common/ServerExports.h"
#include "Logger.h"
#include "sol/sol.hpp"

class LuaPlugin : public Singleton<LuaPlugin> {
private:
	//lua_State* m_lua;

	std::shared_ptr<sol::state> m_lua;

public:
	LuaPlugin();
	~LuaPlugin();
	sol::table initLua(const char* funcName);
	static LuaPlugin* getLuaPlugin();
	inline std::shared_ptr<sol::state> getLua() { return m_lua; }

	template <typename... Args>
	sol::protected_function_result callLuaFunc(const char* modName, const char* funcName, Args&&... args) {
		sol::protected_function func;
		if (modName == NULL) {
			func = m_lua->get<sol::protected_function>(funcName);
		}
		else {
			sol::table mod = m_lua->get<sol::table>(modName);
			if (!mod.valid()) {
				LOG_ERROR("lua module not exist: %s", modName);
				return sol::protected_function_result(NULL, -1, 0, 0, sol::call_status::runtime);
			}
			func = mod.get<sol::protected_function>(funcName);
		}
		if (!func.valid()) {
			LOG_ERROR("call lua func %s.%s invalid", modName == NULL ? "" : modName, funcName);
			return sol::protected_function_result(NULL, -1, 0, 0, sol::call_status::runtime);
		}
		//func.set_default_handler(((*m_lua)["got_problems"]));
		//func.set_default_handler(sol::make_object(m_lua->lua_state(), &sol::default_traceback_error_handler));
		return LuaPlugin::callLuaFunc(func, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static sol::protected_function_result callLuaFunc(sol::protected_function func, Args&&... args) {
		sol::protected_function_result result = func(std::forward<Args>(args)...);
		if (!result.valid()) {
			LOG_ERROR("lua result = %d", result.status());
			/*for (auto iter = result1.begin(); iter != result1.end(); iter++) {
				std::string s = iter->operator std::string();
				LOG_ERROR("%s", s.c_str());
			}*/

			sol::error err = result;
			LOG_ERROR("%s", err.what());
		}
		return result;
	}
};

//
//#ifdef WIN32
//#ifdef SERVER_EXPORT
////INIT_SINGLETON_CLASS(LuaPlugin);
////LuaPlugin * LuaPlugin::_singleon = NULL;
//#endif // SERVER_EXPORT
//#endif // WIN32
