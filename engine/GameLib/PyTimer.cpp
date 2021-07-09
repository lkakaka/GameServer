#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "PyTimer.h"
#include "Logger.h"
#include "Timer.h"
#include <functional>
#include "Profile/ProfileTrack.h"
#include "../Common/PyCommon.h"

static PyObject* ModuleError;
static const char* ModuleName = "Timer";

class _CallbackHander {
public:
	PyObject* m_cb;
	_CallbackHander(PyObject* cb) {
		m_cb = cb;
		Py_INCREF(m_cb);
	}
	~_CallbackHander() {
		auto py_state = PyGILState_Ensure();
		Py_DECREF(m_cb);
		PyGILState_Release(py_state);
		/*if (PyGILState_Check() > 0) {
			Logger::logInfo("$_CallbackHander------------");
		}*/
	}
};

static PyObject* pyAddTimer(PyObject* self, PyObject* args)
{
	int firstInterval = 0;
	int interval = 0;
	int loopCnt = 0;
	PyObject* callback = NULL;
	if (!PyArg_ParseTuple(args, "iiiO", &firstInterval, &interval, &loopCnt, &callback)) {
		PyErr_SetString(ModuleError, "args error");
		return Py_BuildValue("l", -1);
	}

	if (!PyCallable_Check(callback)) {
		PyErr_SetString(ModuleError, "args callback error");
		return Py_BuildValue("l", -1);
	}

	std::shared_ptr<_CallbackHander> callbackHander(new _CallbackHander(callback));

	long timerId = TimerMgr::getSingleton()->addTimer(firstInterval, interval, loopCnt, [callbackHander](int timerId){
		auto py_state = PyGILState_Ensure();
		{
			Logger::logInfo("$execute py timer:%d", timerId);
			PROFILE_TRACK_WITH_TIME("py_timer", 10);
			PyObject* result = PyObject_CallObject(callbackHander->m_cb, NULL);
			if (result == NULL)
			{
				logPyException();
			}
		}
		PyGILState_Release(py_state);
		});
	Logger::logInfo("$add py timer:%d", timerId);
	return Py_BuildValue("l", timerId);
}

static PyObject* pyRemoveTimer(PyObject* self, PyObject* args)
{
	long timerId = 0;
	if (!PyArg_ParseTuple(args, "l", &timerId)) {
		PyErr_SetString(ModuleError, "args error");
		Py_RETURN_FALSE;
	}
	Logger::logInfo("$remove py timer:%d", timerId);
	TimerMgr::getSingleton()->removeTimer(timerId, true);
	Py_RETURN_TRUE;
}

static PyMethodDef module_methods[] = {
	{"addTimer", (PyCFunction)pyAddTimer, METH_VARARGS, ""},
	{"removeTimer", (PyCFunction)pyRemoveTimer, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python timer interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Timer(void)
{
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Timer.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}

void initTimerModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Timer);  // python3
}

