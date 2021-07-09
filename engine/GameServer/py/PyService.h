#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"

class PyService
{
public:
	PyObject_HEAD
};

bool addPyServiceObj(PyObject* module);

