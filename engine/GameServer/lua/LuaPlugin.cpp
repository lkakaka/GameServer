#include "LuaPlugin.h"

//#ifndef WIN32
INIT_SINGLETON_CLASS(LuaPlugin)
//#endif

LuaPlugin::LuaPlugin() : m_lua(NULL) {
	
}

LuaPlugin::~LuaPlugin() {
	if (m_lua != NULL) lua_close(m_lua);                //�رջ���
	m_lua = NULL;
}

void LuaPlugin::initLua() {
	//1.����һ��state
	m_lua = luaL_newstate();
	luaL_openlibs(m_lua);            //��lua��׼��
	//lua_register(L, "foo", foo); //ע��c������lua����
	luaL_dofile(m_lua, "../script/lua/main.lua");     //ִ��lua�ű�
}

LuaPlugin* LuaPlugin::getLuaPlugin() {
	return LuaPlugin::getSingleton();
}