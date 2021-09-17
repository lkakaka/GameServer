#include "PyCommon.h"
#include "Logger.h"

PyObject* callPyObjFunc(PyObject* scriptObj, const char* funcName, PyObject* args) {
	auto func = PyObject_GetAttrString(scriptObj, funcName);
	PyObject* result = PyObject_Call(func, args, NULL);
	if (result == NULL) {
		logPyException();
		//PyErr_Print();
	}
	return result;
}

PyObject* callPyFunction(const char* module, const char* func, PyObject* arg)
{
	PyObject* pModule = NULL;//声明变量
	PyObject* pFunc = NULL;// 声明变量
	pModule = PyImport_ImportModule(module);//这里是要调用的文件名
	if (pModule == NULL)
	{
		PyErr_Print();
		LOG_ERROR("call py function(%s.%s) falied, module is null", module, func);
		return NULL;
	}
	pFunc = PyObject_GetAttrString(pModule, func);//这里是要调用的函数名
	if (pFunc == NULL) {
		PyErr_Print();
		LOG_ERROR("call py function(%s.%s) falied, func not found", module, func);
		return NULL;
	}
	PyObject* result = PyEval_CallObject(pFunc, arg);//调用函数
	//PyObject* result = PyEval_CallMethod(pModule, func, "");

	if (result == NULL)
	{
		logPyException();
	}
	return result;
}

void logPyException() {
	PyObject* ptype, * pvalue, * ptraceback;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);
	PyErr_Restore(ptype, pvalue, ptraceback);
	PyErr_Print();
	PyErr_NormalizeException(&ptype, &pvalue, &ptraceback); // pvalue修改为format_exception可以使用的excettion类型
	PyObject* module_name = PyUnicode_FromString("traceback");
	PyObject* tb_mod = PyImport_Import(module_name);
	Py_DECREF(module_name);
	PyObject* format_func = PyObject_GetAttrString(tb_mod, "format_exception");
	if (format_func && PyCallable_Check(format_func)) {
		PyObject* exList = PyObject_CallFunctionObjArgs(format_func, ptype, pvalue, ptraceback, NULL);
		if (exList == NULL) {
			PyErr_Print();
			LOG_ERROR("call traceback.format_exception failed when log py exception");
			return;
		}
		std::string fullMsg = "";
		for (int i = 0; i < PyList_GET_SIZE(exList); i++) {
			PyObject* exMsg = PyList_GetItem(exList, i);
			fullMsg += PyUnicode_AsUTF8(exMsg);
		}
		Logger::logError(fullMsg.c_str());
		Py_DECREF(exList);
	}
	
	/*PyObject* tb = ptraceback;
		Py_INCREF(tb);
		std::string traceback;
		while (tb != NULL) {
			PyObject* frame = PyObject_GetAttrString(tb, "tb_frame");
			PyObject* f_code = PyObject_GetAttrString(frame, "f_code");
			PyObject* line = PyObject_GetAttrString(frame, "co_firstlineno");
			int line_no = PyLong_AsLong(line);

			PyTracebackObject* t_tb = (PyTracebackObject*)tb;
			int line_no = ((PyTracebackObject*)tb)->tb_frame->f_code->co_firstlineno;
			tb = PyObject_GetAttrString(tb, "tb_next");
		}*/
}