#pragma once
#include <string>
#include "Python.h"


enum ServiceType {
	SERVICE_TYPE_UNKNOW = -1,
	SERVICE_TYPE_START = 0,
	// �������Ͷ���
	SERVIE_TYPE_ROUTER = 1,
	SERVIE_TYPE_GATEWAY = 2,
	SERVIE_TYPE_LOGIN = 3,
	SERVIE_TYPE_DB = 4,
	SERVIE_TYPE_SCENE = 5,
	SERVIE_TYPE_SCENE_CTRL = 6,
	// �������Ͷ������(����������Ҫ�޸�SERVICE_TYPE_END)
	SERVICE_TYPE_END = 7,
};

ServiceType getServiceType(std::string serviceName);

class GameService {
private:
	ServiceType m_serviceType;
public:
	static GameService* g_gameService;
	std::string service_name;
	PyObject* m_scriptObj;

	GameService(std::string service_name, ServiceType serviceType, PyObject* scriptObj);
	PyObject* callPyFunc(const char* funcName, PyObject* args);

	inline ServiceType getServieType() { return m_serviceType; }
};


