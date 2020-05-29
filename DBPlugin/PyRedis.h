#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "../Common/ServerExports.h"
#include "Redis.h"

USING_DATA_BASE;

class PyRedisObj {
public:
	PyObject_HEAD
	Redis* redis;
};

bool addPyRedisObj(PyObject* module);
