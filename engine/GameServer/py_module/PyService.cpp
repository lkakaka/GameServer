#include "PyService.h"
#include "Logger.h"
#include "ServiceType.h"
#include "ServiceInfo.h"
#include "Network/ServiceCommEntityMgr.h"

static PyTypeObject PyService_Type;

static PyObject* PyService_New(struct _typeobject* tobj, PyObject* args, PyObject* obj2) {
	//PyObject* scriptObj;
	//if (!PyArg_ParseTuple(args, "O", &scriptObj)) {
	//	//PyErr_SetString(ModuleError, "create scene obj failed");
	//	Logger::logError("$create py service failed, arg error");
	//	return NULL;
	//}
	PyObject* obj = PyType_GenericNew(tobj, args, obj2);
	return obj;
}

static void PyService_Free(void* ptr) {
	PyObject_Del(ptr);
}


static PyObject* sendMsgToClient(PyObject* self, PyObject* args)
{
	int connId;
	int msgId;
	Py_ssize_t msgLen;
	char* msg = NULL;
	if (!PyArg_ParseTuple(args, "iiy#", &connId, &msgId, &msg, &msgLen)) {
		//PyErr_SetString(ModuleError, "sendMessage failed");
		Logger::logError("$send msg to client error, args error");
		Py_RETURN_FALSE;
	}

	//MessageMgr::sendToClient(connId, msgId, msg, msgLen);

	MyBuffer buffer;
	buffer.writeInt(msgId);
	buffer.writeInt(connId);
	buffer.writeString(msg, msgLen);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_GATEWAY, 0);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());

	Py_RETURN_TRUE;
}

static PyObject* sendMsgToService(PyObject* self, PyObject* args)
{
	PyObject* serviceAddr = NULL;
	int msgId;
	Py_ssize_t msgLen;
	char* msg = NULL;
	if (!PyArg_ParseTuple(args, "Oiy#", &serviceAddr, &msgId, &msg, &msgLen)) {
		Logger::logError("$send msg to service error, args error");
		//PyErr_SetString(ModuleError, "sendMessageToService failed");
		Py_RETURN_FALSE;
	}

	if (serviceAddr == NULL) {
		Logger::logError("$send msg to service error, service addr is null");
		Py_RETURN_FALSE;
	}

	PyObject* pyServiceGroup = PyObject_GetAttrString(serviceAddr, "service_group");
	PyObject* pyServiceType = PyObject_GetAttrString(serviceAddr, "service_type");
	PyObject* pyServiceId = PyObject_GetAttrString(serviceAddr, "service_id");
	int serviceGroup = PyLong_AsLong(pyServiceGroup);
	int serviceType = PyLong_AsLong(pyServiceType);
	int serviceId = PyLong_AsLong(pyServiceId);

	ServiceAddr addr(serviceGroup, serviceType, serviceId);
	//MessageMgr::sendToServer(&addr, msgId, msg, msgLen);

	MyBuffer buffer;
	buffer.writeInt(msgId);
	// 发往gateway的消息都需要一个connId
	if (addr.getServiceType() == SERVICE_TYPE_GATEWAY) buffer.writeInt(-1);
	buffer.writeString(msg, msgLen);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());

	Logger::logInfo("$send msg to service %s, msgId:%d", addr.getName(), msgId);
	Py_RETURN_TRUE;
}

static PyMethodDef tp_methods[] = {
	{"sendMsgToClient", (PyCFunction)sendMsgToClient, METH_VARARGS, ""},
	{"sendMsgToService", (PyCFunction)sendMsgToService, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};


static void initPyService_Type()
{
	memset(&PyService_Type, 0, sizeof(PyService_Type));
	PyService_Type.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	PyService_Type.tp_name = "Game.Service";
	PyService_Type.tp_basicsize = sizeof(PyService);
	PyService_Type.tp_getattro = PyObject_GenericGetAttr;
	PyService_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	PyService_Type.tp_methods = tp_methods;
	PyService_Type.tp_new = PyService_New;
	PyService_Type.tp_free = PyService_Free;
}

bool addPyServiceObj(PyObject* module) {
	initPyService_Type();
	if (PyType_Ready(&PyService_Type) < 0) {
		Logger::logError("$add py service error, ready type failed");
		return false;
	}

	Py_INCREF(&PyService_Type);
	if (PyModule_AddObject(module, "Service", (PyObject*)& PyService_Type) < 0) {
		Py_DECREF(&PyService_Type);
		Logger::logError("$add py service error, add failed");
		return false;
	}
	return true;
}