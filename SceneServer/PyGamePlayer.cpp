#include "PyGamePlayer.h"

#include "Logger.h"
#include "SceneMgr.h"

static PyTypeObject PyPlayer_Type;

static PyObject* PyPlayer_New(struct _typeobject* tobj, PyObject* args, PyObject* obj2) {
	PyObject* tuple;
	PyObject* scriptObj;
	if (!PyArg_ParseTuple(args, "OO", &tuple, &scriptObj)) {
		//PyErr_SetString(ModuleError, "create scene obj failed");
		Logger::logError("$new py player failed, arg error");
		Py_RETURN_NONE;
	}

	PyObject* sceneUidObj = PyTuple_GetItem(tuple, 0);
	PyObject* actorIdObj = PyTuple_GetItem(tuple, 1);

	int sceneUid = PyLong_AsLong(sceneUidObj);
	int actorId = PyLong_AsLong(actorIdObj);

	GameScene* gameScene = SceneMgr::getSceneMgr()->getScene(sceneUid);
	if (gameScene == NULL) {
		Logger::logError("$new py player failed, not found scene, sceneUid:%d", sceneUid);
		Py_RETURN_NONE;
	}

	GameActor* gameActor = gameScene->getActor(actorId);
	if (gameActor == NULL) {
		Logger::logError("$new py player failed, not found actor, sceneUid:%d,actorId:%d", sceneUid, actorId);
		Py_RETURN_NONE;
	}
	GamePlayer* gamePlayer = dynamic_cast<GamePlayer*>(gameActor);
	if (gamePlayer == NULL) {
		Logger::logError("$new py player failed, not found player, sceneUid:%d,actorId:%d", sceneUid, actorId);
		Py_RETURN_NONE;
	}
	/*GamePlayer* gamePlayer = (GamePlayer*)ptr;*/
	gamePlayer->setScriptObj(scriptObj);
	PyObject* obj = PyType_GenericNew(tobj, args, obj2);
	((PyGamePlayer*)obj)->gamePlayer = gamePlayer;
	return obj;
}

static void PyPlayer_Free(void* ptr) {
	PyObject_Del(ptr);
}


static PyMethodDef tp_methods[] = {
	//{"getSceneById", getSceneById, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};


static void initPyGamePlayer_Type()
{
	memset(&PyPlayer_Type, 0, sizeof(PyPlayer_Type));
	PyPlayer_Type.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	PyPlayer_Type.tp_name = "Scene.Player";
	PyPlayer_Type.tp_basicsize = sizeof(PyGamePlayer);
	PyPlayer_Type.tp_getattro = PyObject_GenericGetAttr;
	PyPlayer_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	PyPlayer_Type.tp_methods = tp_methods;
	PyPlayer_Type.tp_new = PyPlayer_New;
	PyPlayer_Type.tp_free = PyPlayer_Free;
}

bool addPyGamePlayer(PyObject* module) {
	initPyGamePlayer_Type();
	if (PyType_Ready(&PyPlayer_Type) < 0) {
		Logger::logError("$add py player obj error, ready type failed");
		return false;
	}

	Py_INCREF(&PyPlayer_Type);
	if (PyModule_AddObject(module, "Player", (PyObject*)& PyPlayer_Type) < 0) {
		Py_DECREF(&PyPlayer_Type);
		Logger::logError("$add py player obj error, add failed");
		return false;
	}
	return true;
}