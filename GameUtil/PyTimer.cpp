#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "PyTimer.h"
#include "Logger.h"
#include "Timer.h"
#include <functional>
#include "Profile/ProfileTrack.h"

static PyObject* ModuleError;
static char* ModuleName = "Timer";

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

	long timerId = TimerMgr::getTimerInstance()->addTimer(firstInterval, interval, loopCnt, [callback](int timerId){
		auto py_state = PyGILState_Ensure();
		{
			PROFILE_TRACK_WITH_TIME("py_timer", 10);
			PyObject_CallObject(callback, NULL);
		}
		PyGILState_Release(py_state);
		});
	return Py_BuildValue("l", timerId);
}

static PyObject* pyRemoveTimer(PyObject* self, PyObject* args)
{
	long timerId = 0;
	if (!PyArg_ParseTuple(args, "l", &timerId)) {
		PyErr_SetString(ModuleError, "args error");
		Py_RETURN_FALSE;
	}
	TimerMgr::getTimerInstance()->removeTimer(timerId, true);
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

