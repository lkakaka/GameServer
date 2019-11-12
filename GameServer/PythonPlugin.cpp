#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "PythonPlugin.h"
#include "PyDbInterface.h"
#include "Logger.h"

static PyObject* TestError;

static PyObject* addList_add(PyObject* self, PyObject* args) {

	PyObject* listObj;

	//The input arguments come as a tuple, we parse the args to get the various variables
	//In this case it's only one list variable, which will now be referenced by listObj
	if (!PyArg_ParseTuple(args, "O", &listObj))
		return NULL;

	//length of the list
	long length = PyList_Size(listObj);

	//iterate over all the elements
	int i, sum = 0;
	for (i = 0; i < length; i++) {
		//get an element out of the list - the element is also a python objects
		PyObject* temp = PyList_GetItem(listObj, i);
		//we know that object represents an integer - so convert it into C long
		long elem = PyLong_AsLong(temp);
		sum += elem;
	}

	//value returned back to python code - another python object
	//build value here converts the C long to a python integer
	return Py_BuildValue("i", sum);

}

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

//static PyObject* initTable(PyObject* self, PyObject* args)
//{
//	char* tbName = NULL;
//	PyObject* fieldTuple;
//	if (!PyArg_ParseTuple(args, "sO", &tbName, &fieldTuple)) {
//		PyErr_SetString(TestError, "init table failed");
//		Py_RETURN_FALSE;
//	}
//	long length = PyTuple_Size(fieldTuple);
//	for (int i = 0; i < length; i++) {
//		//get an element out of the list - the element is also a python objects
//		PyObject* fieldInfo = PyTuple_GetItem(fieldTuple, i);
//		PyObject* val = PyDict_GetItemString(fieldInfo, "fieldName");
//		char* fieldName = PyBytes_AsString(val);
//		val = PyDict_GetItemString(fieldInfo, "filedType");
//		int fieldType = PyLong_AsLong(val);
//		Logger::logInfo("$init table %s, field:%s, fieldType:%d", tbName, fieldName, fieldType);
//	}
//	Py_RETURN_TRUE;
//}

static PyMethodDef module_methods[] = {
	{"add", (PyCFunction)addList_add, METH_VARARGS, ""},
	{"test", (PyCFunction)test, METH_VARARGS, ""},
	//{"initTable", (PyCFunction)initTable, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	"Test", /* name of module */
	"usage: Combinations.uniqueCombinations(lstSortableItems, comboSize)\n", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Test(void)
{
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	TestError = PyErr_NewException("Test.error", NULL, NULL);
	Py_XINCREF(TestError);
	if (PyModule_AddObject(moudle, "error", TestError) < 0) {
		Py_XDECREF(TestError);
		Py_CLEAR(TestError);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}


static void callPyFunction(char* module, char* func)
{
	PyObject* pModule = NULL;//声明变量
	PyObject* pFunc = NULL;// 声明变量
	pModule = PyImport_ImportModule(module);//这里是要调用的文件名
	if (pModule == NULL)
	{
		PyErr_Print();
		Logger::logInfo("$call py function(%s.%s) falied, module is null", module, func);
		return;
	}
	pFunc = PyObject_GetAttrString(pModule, func);//这里是要调用的函数名
	if (pFunc == NULL) {
		PyErr_Print();
		Logger::logInfo("$call py function(%s.%s) falied, func not found", module, func);
		return;
	}
	PyObject* result = PyEval_CallObject(pFunc, NULL);//调用函数
	if (result == NULL)
	{
		PyErr_Print();
	}
}

void initPython()
{
	PyImport_AppendInittab("Test", PyInit_Test);  // python3
	initDbModule();
	Py_Initialize();
	//Py_InitModule("Test", module_methods);	// python2
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	PyRun_SimpleString("print(os.getcwd())");
	PyRun_SimpleString("sys.path.append(os.path.abspath('..') + '\\script\\python')");
	PyRun_SimpleString("print(sys.path)");
	callPyFunction("main", "init");
}


void finalizePython()
{
	Py_Finalize();
}
