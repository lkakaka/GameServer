#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "PyLogger.h"
#include "Logger.h"

static PyObject* ModuleError;
static char* ModuleName = "Logger";

static PyObject* logInfo(PyObject* self, PyObject* args)
{
	char* msg = NULL;
	if (!PyArg_ParseTuple(args, "s", &msg)) {
		PyErr_SetString(ModuleError, "logInfo failed");
		Py_RETURN_FALSE;
	}

	Logger::logInfo(msg);
	Py_RETURN_TRUE;
}

static PyObject* logError(PyObject* self, PyObject* args)
{
	char* msg = NULL;
	if (!PyArg_ParseTuple(args, "s", &msg)) {
		PyErr_SetString(ModuleError, "logError failed");
		Py_RETURN_FALSE;
	}

	Logger::logError(msg);
	Py_RETURN_TRUE;
}

static PyObject* logDebug(PyObject* self, PyObject* args)
{
	char* msg = NULL;
	if (!PyArg_ParseTuple(args, "s", &msg)) {
		PyErr_SetString(ModuleError, "logDebug failed");
		Py_RETURN_FALSE;
	}

	Logger::logDebug(msg);
	Py_RETURN_TRUE;
}

static PyMethodDef module_methods[] = {
	{"logInfo", (PyCFunction)logInfo, METH_VARARGS, ""},
	{"logError", (PyCFunction)logError, METH_VARARGS, ""},
	{"logDebug", (PyCFunction)logDebug, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python logger interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Logger(void)
{
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Logger.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}

void initLoggerModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Logger);  // python3
}

