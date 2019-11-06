#include "Python.h"
#include "PythonPlugin.h"

void initPython()
{
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	PyRun_SimpleString("print(os.getcwd())");
	PyRun_SimpleString("sys.path.append('../script/python')");
	PyObject* pModule = NULL;//声明变量
	PyObject* pFunc = NULL;// 声明变量
	pModule = PyImport_ImportModule("main");//这里是要调用的文件名
	pFunc = PyObject_GetAttrString(pModule, "init");//这里是要调用的函数名
	PyEval_CallObject(pFunc, NULL);//调用函数
}

void finalizePython()
{
	Py_Finalize();
}