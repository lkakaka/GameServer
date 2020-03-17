#include "PySceneObj.h"
#include "Logger.h"
#include "SceneMgr.h"

static PyTypeObject PySceneObj_Type;

static PyObject* PySceneObj_New(struct _typeobject* tobj, PyObject* args, PyObject* obj2) {
	int sceneId;
	PyObject* scriptObj;
	if (!PyArg_ParseTuple(args, "iO", &sceneId, &scriptObj)) {
		//PyErr_SetString(ModuleError, "create scene obj failed");
		Logger::logError("$create scene obj failed, arg error");
		Py_RETURN_NONE;
	}
	GameScene* gameScene = SceneMgr::getSceneMgr()->createScene(sceneId, scriptObj);
	if (gameScene == NULL) {
		Logger::logError("$create scene obj failed, db handler exist, sceneId:%d", sceneId);
		Py_RETURN_NONE;
	}
	PyObject* obj = PyType_GenericNew(tobj, args, obj2);
	((PySceneObj*)obj)->gameScene = gameScene;
	return obj;
}

static void PySceneObj_Free(void* ptr) {
	GameScene* gameScene = ((PySceneObj*)ptr)->gameScene;
	//printf("PySceneObj_Free-=--\n");
	SceneMgr::getSceneMgr()->destroyScene(gameScene->getSceneUid());
	PyObject_Del(ptr);
}

static PyObject* createPlayer(PyObject* self, PyObject* args)
{
	int connId, roleId;
	char* name;
	int x, y;
	PyObject* scriptObj;
	if (!PyArg_ParseTuple(args, "iisii", &connId, &roleId, &name, &x, &y)) {
		//PyErr_SetString(ModuleError, "create scene obj failed");
		Logger::logError("$create player failed, arg error");
		Py_RETURN_NONE;
	}
	GameScene* gameScene = ((PySceneObj*)self)->gameScene;
	GamePlayer* gamePlayer = gameScene->createPlayer(connId, roleId, name, x, y);
	//return PyLong_FromSize_t((long long)gamePlayer);
	PyObject* tuple = PyTuple_New(2);
	PyTuple_SetItem(tuple, 0, PyLong_FromLong(gameScene->getSceneUid()));
	PyTuple_SetItem(tuple, 1, PyLong_FromLong(gamePlayer->getActorId()));
	return tuple;
}

static PyObject* removeActor(PyObject* self, PyObject* args)
{
	int actorId;
	if (!PyArg_ParseTuple(args, "i", &actorId)) {
		Logger::logError("$remove actor failed, arg error");
		Py_RETURN_FALSE;
	}
	GameScene* gameScene = ((PySceneObj*)self)->gameScene;
	gameScene->removeActor(actorId);
	Py_RETURN_TRUE;
}

static PyObject* onPlayerEnter(PyObject* self, PyObject* args)
{
	int actorId;
	if (!PyArg_ParseTuple(args, "i", &actorId)) {
		Logger::logError("$on player enter failed, arg error");
		Py_RETURN_FALSE;
	}
	GameScene* gameScene = ((PySceneObj*)self)->gameScene;
	gameScene->onActorEnter(actorId);
	Py_RETURN_TRUE;
}

static PyMethodDef tp_methods[] = {
	//{"getSceneById", getSceneById, METH_VARARGS, ""},
	{"createPlayer", (PyCFunction)createPlayer, METH_VARARGS, ""},
	{"removeActor", (PyCFunction)removeActor, METH_VARARGS, ""},
	{"onPlayerEnter", (PyCFunction)onPlayerEnter, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};


static void initPySceneObj_Type()
{
	memset(&PySceneObj_Type, 0, sizeof(PySceneObj_Type));
	PySceneObj_Type.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	PySceneObj_Type.tp_name = "Scene.SceneObj";
	PySceneObj_Type.tp_basicsize = sizeof(PySceneObj);
	PySceneObj_Type.tp_getattro = PyObject_GenericGetAttr;
	PySceneObj_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	PySceneObj_Type.tp_methods = tp_methods;
	PySceneObj_Type.tp_new = PySceneObj_New;
	PySceneObj_Type.tp_free = PySceneObj_Free;
}

bool addPySceneObj(PyObject* module) {
	initPySceneObj_Type();
	if (PyType_Ready(&PySceneObj_Type) < 0) {
		Logger::logError("$add py scene obj error, ready type failed");
		return false;
	}

	Py_INCREF(&PySceneObj_Type);
	if (PyModule_AddObject(module, "SceneObj", (PyObject*)& PySceneObj_Type) < 0){
		Py_DECREF(&PySceneObj_Type);
		Logger::logError("$add py scene obj error, add failed");
		return false;
	}
	return true;
}