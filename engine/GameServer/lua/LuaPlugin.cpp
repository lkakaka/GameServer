#include "LuaPlugin.h"

void LuaPlugin::initLua() {
	//1.����һ��state
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);            //��lua��׼��
	//lua_register(L, "foo", foo); //ע��c������lua����
	luaL_dofile(L, "../script/lua/main.lua");     //ִ��lua�ű�
	lua_close(L);                //�رջ���
}