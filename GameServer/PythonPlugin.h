#pragma once

#include "PyCustomObj.h"

void initPython();
void finalizePython();
PyObject* callPyFunction(const char* module, const char* func, PyObject* arg);
