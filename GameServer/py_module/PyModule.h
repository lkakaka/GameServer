#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"

void initLoggerModule();
void addLoggerObj();
void initGameModule();
void initConfigModule();
