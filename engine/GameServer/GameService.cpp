#include "GameService.h"
#include "PyCommon.h"
#include "PythonPlugin.h"
#include "Logger.h"


GameService* GameService::g_gameService = NULL;

GameService::GameService(std::string service_name, ServiceType serviceType) :
	service_name(service_name), m_serviceType(serviceType), m_PyObj(NULL), m_luaObj(sol::table())
{
	
}

GameService::~GameService() {
	
}

void GameService::initScript(const char* funcName) {
	if (funcName == NULL || strlen(funcName) == 0) return;
#ifdef USE_PYTHON_SCRIPT
	PythonPlugin::initPython();
	auto py_state = PyGILState_Ensure();
	m_PyObj = callPyFunction("main", funcName, NULL);
	PyGILState_Release(py_state);
#else
	new LuaPlugin();
	m_luaObj = LuaPlugin::getLuaPlugin()->initLua(funcName);
#endif // USE_PYTHON_SCRIPT	
}

PyObject* GameService::callPyFunc(const char* funcName, PyObject* args) {
	auto func = PyObject_GetAttrString(m_PyObj, funcName);
	PyObject* obj = PyObject_Call(func, args, NULL);
	if (obj == NULL) {
		//PyErr_Print();
		logPyException();
	}
	return obj;
}

//sol::protected_function_result GameService::callLuaFunc(const char* funcName, ...) {
//	sol::function func = m_luaObj.get<sol::function>(funcName);
//	func(std::move();
//}

void GameService::dispatchClientMsgToScript(int connId, int msgId, const char* data, int len) {
#ifdef USE_PYTHON_SCRIPT
	auto py_state = PyGILState_Ensure();
	PyObject* arg = PyTuple_New(3);
	PyTuple_SetItem(arg, 0, PyLong_FromLong(connId));
	PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
	PyTuple_SetItem(arg, 2, Py_BuildValue("y#", data, len));
	callPyFunc("on_recv_client_msg", arg);
	Py_INCREF(arg);
	PyGILState_Release(py_state);
#else
	char* buff = new char[len + 1]{ 0 };
	memcpy(buff, data, len);
	sol::function func = m_luaObj.get<sol::function>("on_recv_client_msg");
	sol::protected_function_result result = func(m_luaObj, connId, msgId, buff);
	delete[] buff;
	if (!result.valid()) {
		Logger::logError("$lua result = %d", result.status());
		sol::error err = result;
		std::string what = err.what();
		std::cout << what << std::endl;
		Logger::logError("$%s", err.what());
	}

#endif // USE_PYTHON_SCRIPT

	
}

void GameService::dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len) {
#ifdef USE_PYTHON_SCRIPT
	auto py_state = PyGILState_Ensure();
	PyObject* arg = PyTuple_New(3);
	PyObject* pArgs = Py_BuildValue("iii", srcAddr->getServiceGroup(), srcAddr->getServiceType(), srcAddr->getServiceId());
	PyObject* pyObj = callPyFunc("create_service_addr", pArgs);
	PyTuple_SetItem(arg, 0, pyObj);
	PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
	PyTuple_SetItem(arg, 2, Py_BuildValue("y#", data, len));
	callPyFunc("on_recv_service_msg", arg);
	Py_INCREF(arg);
	Py_INCREF(pArgs);
	Py_INCREF(pyObj);
	PyGILState_Release(py_state);
#else
	char* buff = new char[len+1] {0};
	memcpy(buff, data, len);
	sol::function func = m_luaObj.get<sol::function>("on_recv_service_msg");
	sol::protected_function_result result = func(m_luaObj, srcAddr->getName(), msgId, buff);
	delete[] buff;
	if (!result.valid()) {
		Logger::logError("$lua result = %d", result.status());
		sol::error err = result;
		std::string what = err.what();
		std::cout << what << std::endl;
		Logger::logError("$%s", err.what());
	}
#endif // USE_PYTHON_SCRIPT
}
