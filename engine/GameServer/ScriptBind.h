#pragma once

/* todo: LuaPlugin.h必须在Python.h前，否则因为LuaPlugin会包含boost.asio的头文件而抛错
boost_1_70_0\boost/system/detail/ system_category_win32.hpp(52, 10) : error C2039 : "_snprintf" : 不是 "std" 的成员.
即boost.asio的头文件必须在Python.h头文件前，why???
*/
//#include "LuaPlugin.h"
//#include "Python.h"
#include "IScript.h"

class ScriptBind {
public:
	static IScript* bindScript(const char* scriptFunc);
};


