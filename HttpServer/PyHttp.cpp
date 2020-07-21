#include "PyHttp.h"
#include "../Common/PyCommon.h"
#include "Logger.h"
#include "PyHttpServer.hpp"
#include "PyHttpUtil.h"

reply_ptr onRecvHttpReq(void* script_obj, int conn_id, const request& req) {
	auto py_state = PyGILState_Ensure();
	PyObject* scriptObj = (PyObject*)script_obj;
	PyObject* pModule = PyImport_ImportModule("http_util.http_req");//这里是要调用的文件名
	if (pModule == NULL)
	{
		logPyException();
		PyGILState_Release(py_state);
		return NULL;
	}
	PyObject* pFunc = PyObject_GetAttrString(pModule, "HttpReq");//这里是要调用的函数名
	PyObject* http_req = PyEval_CallObject(pFunc, NULL);//调用函数
	if (http_req == NULL) {
		logPyException();
		PyGILState_Release(py_state);
		return NULL;
	}
	PyObject* headers = PyDict_New();
	for (header h : req.headers) {
		PyDict_SetItem(headers, PyUnicode_FromString(h.name.c_str()), PyUnicode_FromString(h.value.c_str()));
	}
	PyObject_SetAttrString(http_req, "method", PyUnicode_FromString(req.method.c_str()));
	PyObject_SetAttrString(http_req, "uri", PyUnicode_FromString(req.uri.c_str()));
	PyObject_SetAttrString(http_req, "headers", headers);

	PyObject* args = PyTuple_New(2);
	PyTuple_SetItem(args, 0, PyLong_FromLong(conn_id));
	PyTuple_SetItem(args, 1, http_req);
	//PyObject* pFunc = PyObject_GetAttrString(pModule, "on_recv_http_req");//这里是要调用的函数名
	PyObject* resp = callPyObjFunc(scriptObj, "on_recv_http_req", args);

	reply_ptr rep = NULL;
	if (resp != NULL && resp != Py_None) {
		rep = getHttpReply(resp);
	}
	//callPyFunction("http_util.http_mgr", "on_recv_http_req", args);
	PyGILState_Release(py_state);
	return rep;
}


static PyObject* ModuleError;
static char* ModuleName = "Http";

static PyObject* createHttpServer(PyObject* self, PyObject* args)
{
	char* ip;
	int port;
	if (!PyArg_ParseTuple(args, "si", &ip, &port)) {
		//PyErr_SetString(ModuleError, "logInfo failed");
		Logger::logError("createHttpServer args error");
		Py_RETURN_NONE;
	}

	Py_RETURN_TRUE;
}

static PyObject* sendHttpResp(PyObject* self, PyObject* args)
{
	int conn_id;
	PyObject* resp = NULL;
	if (!PyArg_ParseTuple(args, "is", &conn_id, &resp)) {
		//PyErr_SetString(ModuleError, "logInfo failed");
		Logger::logError("sendHttpResp args error");
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyMethodDef module_methods[] = {
	{"createHttpServer", (PyCFunction)createHttpServer, METH_VARARGS, ""},
	{"sendHttpResp", (PyCFunction)sendHttpResp, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python http interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Http(void)
{
	PyObject* module = PyModule_Create(&module_def);
	if (module == NULL) {
		Logger::logError("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Http.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(module, "error", ModuleError) < 0) {
		goto error;
	}

	if (!addPyHttpServer(module)) {
		goto error;
	}

	return module;

error:
	Py_XDECREF(ModuleError);
	Py_CLEAR(ModuleError);
	Py_DECREF(module);
	return NULL;
}

void initHttpModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Http);  // python3
}

