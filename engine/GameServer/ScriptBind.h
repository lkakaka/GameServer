#pragma once

/* todo: LuaPlugin.h������Python.hǰ��������ΪLuaPlugin�����boost.asio��ͷ�ļ����״�
boost_1_70_0\boost/system/detail/ system_category_win32.hpp(52, 10) : error C2039 : "_snprintf" : ���� "std" �ĳ�Ա.
��boost.asio��ͷ�ļ�������Python.hͷ�ļ�ǰ��why???
*/
//#include "LuaPlugin.h"
//#include "Python.h"
#include "IScript.h"

class ScriptBind {
public:
	static IScript* bindScript(const char* scriptFunc);
};


