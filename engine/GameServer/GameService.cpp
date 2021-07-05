#include "GameService.h"
#include "../Common/PyCommon.h"


GameService* GameService::g_gameService = NULL;

GameService::GameService(std::string service_name, ServiceType serviceType, PyObject* scriptObj) :
	service_name(service_name), m_serviceType(serviceType), m_scriptObj(scriptObj)
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

void GameService::dispatchClientMsgToScript(int connId, int msgId, const char* data, int len) {
	auto py_state = PyGILState_Ensure();
	PyObject* arg = PyTuple_New(3);
	PyTuple_SetItem(arg, 0, PyLong_FromLong(connId));
	PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
	PyTuple_SetItem(arg, 2, Py_BuildValue("y#", data, len));
	callPyFunc("on_recv_client_msg", arg);
	Py_INCREF(arg);
	PyGILState_Release(py_state);
}

void GameService::dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len) {
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
}