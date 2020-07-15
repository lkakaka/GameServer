#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"

void initPython();
void finalizePython();
