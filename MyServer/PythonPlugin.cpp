#include "Python.h"
#include "PythonPlugin.h"

void initPython()
{
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("import os");
	PyRun_SimpleString("print(os.getcwd())");
	PyRun_SimpleString("sys.path.append('../script/python')");
	PyObject* pModule = NULL;//��������
	PyObject* pFunc = NULL;// ��������
	pModule = PyImport_ImportModule("main");//������Ҫ���õ��ļ���
	pFunc = PyObject_GetAttrString(pModule, "init");//������Ҫ���õĺ�����
	PyEval_CallObject(pFunc, NULL);//���ú���
}

void finalizePython()
{
	Py_Finalize();
}