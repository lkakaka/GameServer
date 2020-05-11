#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "structmember.h"
#include "GameScene.h"

class PySceneObj {
public:
	PyObject_HEAD
	GameScene* gameScene;
	PyObject* scene_uid;
};

bool addPySceneObj(PyObject* module);


