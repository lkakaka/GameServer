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

	sol::protected_function_result callLuaFunc(const char* modName, const char* funcName);

	template <typename... Args>
	static sol::protected_function_result callLuaFunc(sol::protected_function func, Args&&... args) {
		sol::protected_function_result result = func(std::forward<Args>(args)...);
		if (!result.valid()) {
			Logger::logError("$lua result = %d", result.status());
			for (auto iter = result.begin(); iter != result.end(); iter++) {
				std::string s = iter->operator std::string();
				Logger::logError("$%s", s.c_str());
			}

			sol::error err = result;
			std::string what = err.what();
			std::cout << what << std::endl;
			Logger::logError("$%s", err.what());
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
