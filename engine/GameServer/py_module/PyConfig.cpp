
#include "PyModule.h"
#include "Logger.h"
#include "../PythonPlugin.h"
#include "Config.h"

static PyObject* ModuleError;
static char* ModuleName = "Config";
extern std::string g_cfgFileName;

static PyObject* getConfigInt(PyObject* self, PyObject* args)
{
	char* key = NULL;
	if (!PyArg_ParseTuple(args, "s", &key)) {
		//PyErr_SetString(ModuleError, "logInfo failed");
		Logger::logError("get config(int) args error");
		Py_RETURN_NONE;
	}

	int val = Config::getSingleton()->getConfigInt(key);
	return PyLong_FromLong(val);
}

static PyObject* getConfigStr(PyObject* self, PyObject* args)
{
	char* key = NULL;
	if (!PyArg_ParseTuple(args, "s", &key)) {
		//PyErr_SetString(ModuleError, "logInfo failed");
		Logger::logError("get config(str) args error");
		Py_RETURN_NONE;
	}

	std::string val = Config::getSingleton()->getConfigStr(key);
	return PyUnicode_FromString(val.c_str());
}


static PyMethodDef module_methods[] = {
	{"getConfigInt", (PyCFunction)getConfigInt, METH_VARARGS, ""},
	{"getConfigStr", (PyCFunction)getConfigStr, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};


static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python config interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Config(void)
{
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Config.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}

void initConfigModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Config);  // python3
}


