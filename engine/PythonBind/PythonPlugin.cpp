
#include "PythonPlugin.h"
#include "PyDb.h"
#include "Logger.h"
#include "PyModule.h"
#include "PyTimer.h"
#include "PyScene.h"
#include "PyHttp.h"
#include "PyCommon.h"

static PyObject* TestError = NULL;
INIT_SINGLETON_CLASS(PythonPlugin)

PythonPlugin::PythonPlugin(const char* entryFuncName) {
	PythonPlugin::initPython();
	auto py_state = PyGILState_Ensure();
	m_service = callPyFunction("main", entryFuncName, NULL);
	PyGILState_Release(py_state);
}

static PyObject* test(PyObject* self, PyObject* args) 
{
	LOG_INFO("test func");
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
		LOG_ERROR("init module %s failed", module_def.m_name);
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

void PythonPlugin::initPython()
{	
	PyImport_AppendInittab("Test", PyInit_Test);  // python3

	initConfigModule();
	initCryptModule();
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
	// 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。
	PyEval_ReleaseThread(PyThreadState_Get());
}


void PythonPlugin::finalizePython()
{
	Py_Finalize();
}


PyObject* PythonPlugin::callServiceFunc(const char* funcName, PyObject* args) {
	auto func = PyObject_GetAttrString(m_service, funcName);
	PyObject* obj = PyObject_Call(func, args, NULL);
	if (obj == NULL) {
		//PyErr_Print();
		logPyException();
	}
	return obj;
}

void PythonPlugin::initScript() {

}

void PythonPlugin::dispatchClientMsgToScript(int connId, int msgId, const char* data, int len) {
	auto py_state = PyGILState_Ensure();
	PyObject* arg = PyTuple_New(3);
	PyTuple_SetItem(arg, 0, PyLong_FromLong(connId));
	PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
	PyTuple_SetItem(arg, 2, Py_BuildValue("y#", data, len));
	callServiceFunc("on_recv_client_msg", arg);
	Py_INCREF(arg);
	PyGILState_Release(py_state);
}

void PythonPlugin::dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len) {
	auto py_state = PyGILState_Ensure();
	PyObject* arg = PyTuple_New(3);
	PyObject* pArgs = Py_BuildValue("iii", srcAddr->getServiceGroup(), srcAddr->getServiceType(), srcAddr->getServiceId());
	PyObject* pyObj = callServiceFunc("create_service_addr", pArgs);
	PyTuple_SetItem(arg, 0, pyObj);
	PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
	PyTuple_SetItem(arg, 2, Py_BuildValue("y#", data, len));
	callServiceFunc("on_recv_service_msg", arg);
	Py_INCREF(arg);
	Py_INCREF(pArgs);
	Py_INCREF(pyObj);
	PyGILState_Release(py_state);
}
