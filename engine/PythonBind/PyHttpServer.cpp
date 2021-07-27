#include "PyHttpServer.hpp"
#include "Logger.h"
#include "connection.hpp"
#include "PyHttpUtil.h"
//#include "server.hpp"
#include "PyHttp.h"

static PyTypeObject PyHttpServer_Type;

static PyObject* PyHttpServer_New(struct _typeobject* tobj, PyObject* args, PyObject* obj2) {
	int httpServerPort;
	PyObject* scriptObj;
	if (!PyArg_ParseTuple(args, "iO", &httpServerPort, &scriptObj)) {
		//PyErr_SetString(ModuleError, "create scene obj failed");
		Logger::logError("$create http server failed, arg error");
		Py_RETURN_NONE;
	}

	char port[8]{0};
	sprintf(port, "%d", httpServerPort);
	http::server::server* serv = new http::server::server("0.0.0.0", port, "");
	serv->bindPyScriptObject(scriptObj, NULL);
	serv->setCallHttpScripFunc(onRecvHttpReq);
	PyObject* obj = PyType_GenericNew(tobj, args, obj2);
	((PyHttpServer*)obj)->http_server = serv;
	
	std::shared_ptr<std::thread> http_thread;
	http_thread.reset(new std::thread([serv] { serv->run(); }));
	((PyHttpServer*)obj)->http_thread = http_thread;
	//new std::thread([&serv] { serv->run(); });
	Logger::logInfo("$create http server, port:%d", httpServerPort);
	return obj;
}

static void PyHttpServer_Free(void* ptr) {
	http::server::server* serv = ((PyHttpServer*)ptr)->http_server;
	delete serv;
	//printf("PyHttpServer_Free-=--\n");
	PyObject_Del(ptr);
}

static PyObject* sendHttpResp(PyObject* self, PyObject* args)
{
	int connId;
	PyObject* resp;
	if (!PyArg_ParseTuple(args, "iO", &connId, &resp)) {
		//PyErr_SetString(ModuleError, "create scene obj failed");
		Logger::logError("$send http resp failed, arg error");
		Py_RETURN_FALSE;
	}
	http::server::server* http_serv = ((PyHttpServer*)self)->http_server;
	http::server::connection_ptr conn_ptr = http_serv->get_connection_mgr()->get_connection(connId);
	if (conn_ptr == nullptr) {
		Logger::logError("$send http resp failed, connection has disconnect");
		Py_RETURN_FALSE;
	}
	conn_ptr->send_resp(getHttpReply(resp));
	Py_RETURN_TRUE;
}

static PyMethodDef tp_methods[] = {
	{"sendHttpResp", (PyCFunction)sendHttpResp, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};

//static PyMemberDef tp_members[] = {
//	{NULL, NULL}
//};


static void initPyHttpServer_Type()
{
	memset(&PyHttpServer_Type, 0, sizeof(PyHttpServer_Type));
	PyHttpServer_Type.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	PyHttpServer_Type.tp_name = "Http.HttpServer";
	PyHttpServer_Type.tp_basicsize = sizeof(PyHttpServer);
	PyHttpServer_Type.tp_getattro = PyObject_GenericGetAttr;
	PyHttpServer_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	PyHttpServer_Type.tp_methods = tp_methods;
	//PyHttpServer_Type.tp_members = tp_members;
	PyHttpServer_Type.tp_new = PyHttpServer_New;
	PyHttpServer_Type.tp_free = PyHttpServer_Free;
}

bool addPyHttpServer(PyObject* module) {
	initPyHttpServer_Type();
	if (PyType_Ready(&PyHttpServer_Type) < 0) {
		Logger::logError("$add py http server error, ready type failed");
		return false;
	}

	Py_INCREF(&PyHttpServer_Type);
	if (PyModule_AddObject(module, "HttpServer", (PyObject*)&PyHttpServer_Type) < 0) {
		Py_DECREF(&PyHttpServer_Type);
		Logger::logError("$add py http server error, add failed");
		return false;
	}
	return true;
}