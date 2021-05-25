#pragma once
#include <string>
#include "Python.h"
#include "CommEntity.h"
#include "ServiceType.h"

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


