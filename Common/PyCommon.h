#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"

void logPyException();
PyObject* callPyObjFunc(PyObject* scriptObj, const char* funcName, PyObject* args);
PyObject* callPyFunction(const char* module, const char* func, PyObject* arg);
