#include "PyGamePlayer.h"

#include "Logger.h"
#include "SceneMgr.h"

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
	//{"sendMsgToClient", sendMsgToClient, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};


TYPE_CONSTRUTOR(PyTypeGamePlayer){

}

TYPE_METHOD(PyTypeGamePlayer, tp_methods)
TYPE_NEWFUNC(PyTypeGamePlayer, PyPlayer_New)
TYPE_FREEFUNC(PyTypeGamePlayer, PyPlayer_Free)
