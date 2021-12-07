#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "IScript.h"
#include "Singleton.h"

class PythonPlugin : public IScript, public Singleton<PythonPlugin> {

private:
	PyObject* m_service;

	PyObject* callServiceFunc(const char* funcName, PyObject* args);

public:

	PythonPlugin(const char* entryFuncName);

	static void initPython();
	static void finalizePython();

	void initScript();
	void dispatchClientMsgToScript(int connId, int msgId, const char* data, int len);
	void dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len);
};
