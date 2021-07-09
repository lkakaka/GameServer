#pragma once
#include <string>
#include "Python.h"
#include "LuaPlugin.h"
#include "Network/ServiceCommEntity.h"
#include "ServiceType.h"

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


