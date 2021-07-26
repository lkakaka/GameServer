#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"

class PythonPlugin {
public:
	static void initPython();
	static void finalizePython();
};
