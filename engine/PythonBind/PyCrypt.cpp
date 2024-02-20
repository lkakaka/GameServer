
#include "PyModule.h"
#include "Logger.h"
#include "PythonPlugin.h"
#include "crypt/md5.h"
#include "crypt/DisorderID.h"

static PyObject* ModuleError;
static const char* ModuleName = "Crypt";

static PyObject* _MD5(PyObject* self, PyObject* args)
{
	char* str = NULL;
	if (!PyArg_ParseTuple(args, "s", &str)) {
		//PyErr_SetString(ModuleError, "logInfo failed");
		LOG_ERROR("md5 args error");
		Py_RETURN_NONE;
	}

	MD5 md5;
	md5.update(str, strlen(str));
	const char* md5code = md5.hexdigest();

	return PyUnicode_FromString(md5code);
}

static PyObject* generateDisorderId(PyObject* self, PyObject* args)
{
	int id = 0;
	if (!PyArg_ParseTuple(args, "i", &id)) {
		//PyErr_SetString(ModuleError, "logInfo failed");
		LOG_ERROR("generateDisorderId args error");
		Py_RETURN_NONE;
	}

	int disorderId = DisorderID::generate(id);
	if (disorderId < 0) {
		LOG_ERROR("gen disorder id failed!!");
		Py_RETURN_NONE;
	}

	return Py_BuildValue("i", disorderId);
}

static PyMethodDef module_methods[] = {
	{"md5", (PyCFunction)_MD5, METH_VARARGS, ""},
	{"genDisorderId", (PyCFunction)generateDisorderId, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};


static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python crypt interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Crypt(void)
{
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		LOG_ERROR("init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Crypt.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}

void initCryptModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Crypt);  // python3
}


