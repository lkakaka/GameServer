#include "GameService.h"
#include "../Common/PyCommon.h"


GameService* GameService::g_gameService = NULL;

GameService::GameService(std::string service_name, PyObject* scriptObj) : 
	service_name(service_name), m_scriptObj(scriptObj)
{

}

PyObject* GameService::callPyFunc(const char* funcName, PyObject* args) {
	auto func = PyObject_GetAttrString(m_scriptObj, funcName);
	PyObject* obj = PyObject_Call(func, args, NULL);
	if (obj == NULL) {
		//PyErr_Print();
		logPyException();
	}
	return obj;
}