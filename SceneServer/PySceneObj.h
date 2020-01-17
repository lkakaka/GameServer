#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "GameScene.h"

class PySceneObj {
public:
	PyObject_HEAD
	GameScene* gameScene;
};

bool addPySceneObj(PyObject* module);


