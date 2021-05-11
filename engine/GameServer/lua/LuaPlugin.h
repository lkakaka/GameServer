#pragma once
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "Singleton.h"
#include "../Common/ServerExports.h"

class LuaPlugin : public Singleton<LuaPlugin> {
private:
	lua_State* m_lua;


public:
	LuaPlugin();
	~LuaPlugin();
	void initLua();
	static LuaPlugin* getLuaPlugin();
};

//
//#ifdef WIN32
//#ifdef SERVER_EXPORT
////INIT_SINGLETON_CLASS(LuaPlugin);
////LuaPlugin * LuaPlugin::_singleon = NULL;
//#endif // SERVER_EXPORT
//#endif // WIN32
