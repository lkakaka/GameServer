
#include "PythonPlugin.h"
#include "PyDbInterface.h"
//#include "py_module/PyModule.h"
#include "Logger.h"
#include "py_module/PyModule.h"
#include "PyTimer.h"
#include "PyScene.h"

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


PyObject* callPyFunction(const char* module, const char* func, PyObject* arg)
{
	PyObject* pModule = NULL;//声明变量
	PyObject* pFunc = NULL;// 声明变量
	pModule = PyImport_ImportModule(module);//这里是要调用的文件名
	if (pModule == NULL)
	{
		PyErr_Print();
		Logger::logInfo("$call py function(%s.%s) falied, module is null", module, func);
		return NULL;
	}
	pFunc = PyObject_GetAttrString(pModule, func);//这里是要调用的函数名
	if (pFunc == NULL) {
		PyErr_Print();
		Logger::logInfo("$call py function(%s.%s) falied, func not found", module, func);
		return NULL;
	}
	PyObject* result = PyEval_CallObject(pFunc, arg);//调用函数
	//PyObject* result = PyEval_CallMethod(pModule, func, "");

	if (result == NULL)
	{
		PyErr_Print();
	}
	return result;
}

void initPython()
{	
	PyImport_AppendInittab("Test", PyInit_Test);  // python3

	initDbModule();
	initLoggerModule();
	initTimerModule();
	initGameModule();
	initSceneModule();

	Py_Initialize();
	if (!PyEval_ThreadsInitialized()) {
		PyEval_InitThreads();
	}
	//Py_InitModule("Test", module_methods);	// python2
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	//PyRun_SimpleString("print(os.getcwd())");
#ifdef WIN32
	PyRun_SimpleString("sys.path.append(os.path.abspath('../..') + '/script/python')");
	PyRun_SimpleString("sys.path.append(os.path.abspath('../..') + '/Depends/Python38')");
#else
	PyRun_SimpleString("sys.path.append(os.path.abspath('..') + '/script/python')");
	PyRun_SimpleString("sys.path.append(os.path.abspath('..') + '/Depends/Python38')");
#endif
	PyRun_SimpleString("print(sys.path)");
	//callPyFunction("main", funcName, NULL);
	// 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
	PyEval_ReleaseThread(PyThreadState_Get());
}


void finalizePython()
{
	Py_Finalize();
}
