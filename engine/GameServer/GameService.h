#pragma once
#include <string>
/* todo: LuaPlugin.h必须在Python.h前，否则因为LuaPlugin会包含boost.asio的头文件而抛错
boost_1_70_0\boost/system/detail/ system_category_win32.hpp(52, 10) : error C2039 : "_snprintf" : 不是 "std" 的成员.
即boost.asio的头文件必须在Python.h头文件前，why???
*/
#include "LuaPlugin.h"
#include "Python.h"

#include "Network/ServiceCommEntity.h"
#include "ServiceType.h"
#include "sol/sol.hpp"

class GameService {
private:
	ServiceType m_serviceType;
	PyObject* m_PyObj;
	sol::table m_luaObj;

public:
	static GameService* g_gameService;
	std::string service_name;
	
	GameService(std::string service_name, ServiceType serviceType);
	~GameService();
	
	void initScript(const char* scriptFunc);

	PyObject* callPyFunc(const char* funcName, PyObject* args);

	inline ServiceType getServieType() { return m_serviceType; }

	void dispatchClientMsgToScript(int connId, int msgId, const char* data, int len);
	void dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len);
};


