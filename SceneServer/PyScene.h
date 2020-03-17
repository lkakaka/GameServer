#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "SceneExport.h"

SCENE_API void initSceneModule();
PyObject* callPyFunction(PyObject* scriptObj, const char* funcName, PyObject* args);
