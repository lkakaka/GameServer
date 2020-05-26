#include "PyCommon.h"
#include "Logger.h"

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