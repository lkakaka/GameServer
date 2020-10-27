#pragma once

#include "PyTypeBase.h"
#include "GameScene.h"

class PySceneObj {
public:
	PyObject_HEAD
	GameScene* gameScene;
	PyObject* scene_uid;
};

class PyTypeSceneObj : public PyTypeBase {
public:
	DECLARE_CONSTRUTOR(PyTypeSceneObj);
	TYPE_DECALARE_ALL
};

#define MAKE_SCENE_OBJ_TYPE(tp_name, obj_name, tp_size) new PySceneObjType(tp_name, obj_name, tp_size)



