#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "PyDbInterface.h"
#include "Logger.h"

static PyObject* ModuleError;
static char* ModuleName = "PyDb";

static PyObject* initTable(PyObject* self, PyObject* args)
{
	char* tbName = NULL;
	PyObject* fieldTuple;
	if (!PyArg_ParseTuple(args, "sO", &tbName, &fieldTuple)) {
		PyErr_SetString(ModuleError, "init table failed");
		Py_RETURN_FALSE;
	}
	long length = PyTuple_Size(fieldTuple);
	for (int i = 0; i < length; i++) {
		//get an element out of the list - the element is also a python objects
		PyObject* fieldInfo = PyTuple_GetItem(fieldTuple, i);
		PyObject* val = PyDict_GetItemString(fieldInfo, "fieldName");
		char* fieldName = PyBytes_AsString(val);
		val = PyDict_GetItemString(fieldInfo, "filedType");
		int fieldType = PyLong_AsLong(val);
		Logger::logInfo("$init table %s, field:%s, fieldType:%d", tbName, fieldName, fieldType);
	}
	Py_RETURN_TRUE;
}

static PyMethodDef module_methods[] = {
	{"initTable", (PyCFunction)initTable, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python db interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_PyDb(void)
{
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Db.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}

void initDbModule() {
	PyImport_AppendInittab(ModuleName, PyInit_PyDb);  // python3
}

