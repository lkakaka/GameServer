#include "LuaPlugin.h"

//#ifndef WIN32
INIT_SINGLETON_CLASS(LuaPlugin)
//#endif

LuaPlugin::LuaPlugin() : m_lua(NULL) {
	
}

LuaPlugin::~LuaPlugin() {
	if (m_lua != NULL) lua_close(m_lua);                //关闭环境
	m_lua = NULL;
}

void LuaPlugin::initLua() {
	//1.创建一个state
	m_lua = luaL_newstate();
	luaL_openlibs(m_lua);            //打开lua标准库
	//lua_register(L, "foo", foo); //注册c函数到lua环境
	luaL_dofile(m_lua, "../script/lua/main.lua");     //执行lua脚本
}

LuaPlugin* LuaPlugin::getLuaPlugin() {
	return LuaPlugin::getSingleton();
}