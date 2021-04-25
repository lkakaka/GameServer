
#include "PythonPlugin.h"
#include "PyDb.h"
//#include "py_module/PyModule.h"
#include "Logger.h"
#include "py_module/PyModule.h"
#include "PyTimer.h"
#include "PyScene.h"
#include "PyHttp.h"

static PyObject* TestError = NULL;

static PyObject* test(PyObject* self, PyObject* args) 
{
	Logger::logInfo("$test func");
	/*PyErr_SetString(TestError, "System command failed");
	return NULL;*/
	PyObject* obj = PyTuple_New(2);
	PyTuple_SetItem(obj, 0, PyLong_FromLong(10));
	PyTuple_SetItem(obj, 1, PyLong_FromLong(20));
	return obj;
}

static PyMethodDef module_methods[] = {
	{"test", (PyCFunction)test, METH_VARARGS, ""},
	//{"initTable", (PyCFunction)initTable, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	"Test", /* name of module */
	"test module", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Test(void)
{
	PyObject* module = PyModule_Create(&module_def);
	if (module == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	TestError = PyErr_NewException("Game.error", NULL, NULL);
	Py_XINCREF(TestError);
	if (PyModule_AddObject(module, "error", TestError) < 0) {
		Py_XDECREF(TestError);
		Py_CLEAR(TestError);
		Py_DECREF(module);
		return NULL;
	}

	return module;
}

void initPython()
{	
	PyImport_AppendInittab("Test", PyInit_Test);  // python3

	initConfigModule();
	initDbModule();
	initLoggerModule();
	initTimerModule();
	initGameModule();
	initSceneModule();
	initHttpModule();

	Py_Initialize();
	if (!PyEval_ThreadsInitialized()) {
		PyEval_InitThreads();
	}
	//Py_InitModule("Test", module_methods);	// python2
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	//PyRun_SimpleString("print(os.getcwd())");
//#ifdef WIN32
//	PyRun_SimpleString("sys.path.append(os.path.abspath('../../../../Python-3.8.0/PCbuild/amd64'))");
//	PyRun_SimpleString("sys.path.append(os.path.abspath('..') + '/script/python')");
//	PyRun_SimpleString("sys.path.append(os.path.abspath('..') + '/Depends/Python38')");
//	PyRun_SimpleString("sys.path.append(os.path.abspath('../../..') + '/script/python')");
//	PyRun_SimpleString("sys.path.append(os.path.abspath('../../..') + '/Depends/Python38')");
//#else
	PyRun_SimpleString("sys.path.append(os.path.abspath('..') + '/script/python')");
	PyRun_SimpleString("sys.path.append(os.path.abspath('..') + '/Depends/Python38')");
//#endif
	PyRun_SimpleString("print(sys.path)");
	//callPyFunction("main", funcName, NULL);
	// �������߳�ǰִ�У�Ϊ���ͷ�PyEval_InitThreads��õ�ȫ�������������߳̿����޷���ȡ��ȫ������
	PyEval_ReleaseThread(PyThreadState_Get());
}


void finalizePython()
{
	Py_Finalize();
}
