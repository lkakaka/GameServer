#pragma once

#define SOL_ALL_SAFETIES_ON 1

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "Singleton.h"
#include "../Common/ServerExports.h"

#include "sol/sol.hpp"

class LuaPlugin : public Singleton<LuaPlugin> {
private:
	//lua_State* m_lua;

	std::shared_ptr<sol::state> m_lua;

public:
	LuaPlugin();
	~LuaPlugin();
	void initLua();
	static LuaPlugin* getLuaPlugin();

	bool callLuaFunc(const char* modName, const char* funcName);
};

//
//#ifdef WIN32
//#ifdef SERVER_EXPORT
////INIT_SINGLETON_CLASS(LuaPlugin);
////LuaPlugin * LuaPlugin::_singleon = NULL;
//#endif // SERVER_EXPORT
//#endif // WIN32
