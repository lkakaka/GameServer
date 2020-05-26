#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyObject* callPyFunction(const char* module, const char* func, PyObject* arg);
