
#include "PyModule.h"
#include "Logger.h"
#include "PyService.h"

static PyObject* ModuleError;
static const char* ModuleName = "Game";

static PyMethodDef module_methods[] = {
	/*{"sendMsgToClient", (PyCFunction)sendMsgToClient, METH_VARARGS, ""},
	{"sendMsgToService", (PyCFunction)sendMsgToService, METH_VARARGS, ""},*/
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
		LOG_ERROR("init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Game.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		goto error;
	}

	if (!addPyServiceObj(moudle)) {
		goto error;
	}

	return moudle;

error:
	Py_XDECREF(ModuleError);
	Py_CLEAR(ModuleError);
	Py_DECREF(moudle);
	return NULL;
}

void initGameModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Game);  // python3
}

