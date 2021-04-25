#include "LuaPlugin.h"

void LuaPlugin::initLua() {
	//1.创建一个state
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);            //打开lua标准库
	//lua_register(L, "foo", foo); //注册c函数到lua环境
	luaL_dofile(L, "../script/lua/main.lua");     //执行lua脚本
	lua_close(L);                //关闭环境
}