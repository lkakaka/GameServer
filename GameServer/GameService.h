#pragma once
#include <string>
#include "Python.h"

class GameService {
public:
	static GameService* g_gameService;
	std::string service_name;
	PyObject* m_scriptObj;

	GameService(std::string service_name, PyObject* scriptObj);
	PyObject* callPyFunc(const char* funcName, PyObject* args);
};


