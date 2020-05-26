#include "PyCommon.h"
#include "Logger.h"

PyObject* callPyFunction(const char* module, const char* func, PyObject* arg)
{
	PyObject* pModule = NULL;//��������
	PyObject* pFunc = NULL;// ��������
	pModule = PyImport_ImportModule(module);//������Ҫ���õ��ļ���
	if (pModule == NULL)
	{
		PyErr_Print();
		Logger::logInfo("$call py function(%s.%s) falied, module is null", module, func);
		return NULL;
	}
	pFunc = PyObject_GetAttrString(pModule, func);//������Ҫ���õĺ�����
	if (pFunc == NULL) {
		PyErr_Print();
		Logger::logInfo("$call py function(%s.%s) falied, func not found", module, func);
		return NULL;
	}
	PyObject* result = PyEval_CallObject(pFunc, arg);//���ú���
	//PyObject* result = PyEval_CallMethod(pModule, func, "");

	if (result == NULL)
	{
		PyErr_Print();
	}
	return result;
}