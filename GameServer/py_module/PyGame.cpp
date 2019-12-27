
#include "PyModule.h"
#include "Logger.h"
#include "../MessageMgr.h"

static PyObject* ModuleError;
static char* ModuleName = "Game";

static PyObject* sendMessage(PyObject* self, PyObject* args)
{
	int connId;
	int msgId;
	int msgLen;
	char* msg = NULL;
	if (!PyArg_ParseTuple(args, "iiis", &connId, &msgId, &msgLen, &msg)) {
		PyErr_SetString(ModuleError, "sendMessage failed");
		Py_RETURN_FALSE;
	}

	MessageMgr::sendToClient(connId, msgId, msg, msgLen);
	Py_RETURN_TRUE;
}

static PyObject* sendMessageToServer(PyObject* self, PyObject* args)
{
	char* serviceName = NULL;
	int msgId;
	int msgLen;
	char* msg = NULL;
	if (!PyArg_ParseTuple(args, "siis", &serviceName, &msgId, &msgLen, &msg)) {
		PyErr_SetString(ModuleError, "sendMessageToServer failed");
		Py_RETURN_FALSE;
	}

	MessageMgr::sendToServer(serviceName, msgId, msg, msgLen);
	Py_RETURN_TRUE;
}

static PyMethodDef module_methods[] = {
	{"sendMessage", (PyCFunction)sendMessage, METH_VARARGS, ""},
	{"sendMessageToServer", (PyCFunction)sendMessageToServer, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python game interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Game(void)
{
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Game.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}

void initGameModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Game);  // python3
}

