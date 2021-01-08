#include "PySceneObj.h"
#include "Logger.h"
#include "SceneMgr.h"

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
	((PySceneObj*)obj)->scene_uid = PyLong_FromLong(gameScene->getSceneUid());
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
	int x, y, move_speed;
	PyObject* scriptObj;
	if (!PyArg_ParseTuple(args, "iisiii", &connId, &roleId, &name, &x, &y, &move_speed)) {
		//PyErr_SetString(ModuleError, "create scene obj failed");
		Logger::logError("$create player failed, arg error");
		Py_RETURN_NONE;
	}
	GameScene* gameScene = ((PySceneObj*)self)->gameScene;
	GamePlayer* gamePlayer = gameScene->createPlayer(connId, roleId, name, x, y);
	gamePlayer->setMoveSpeed(move_speed);
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

static PyObject* loadNavMesh(PyObject* self, PyObject* args) {
	char* meshFileName;
	if (!PyArg_ParseTuple(args, "s", &meshFileName)) {
		Logger::logError("$loadNavMesh failed, arg error");
		Py_RETURN_FALSE;
	}

	GameScene* gameScene = ((PySceneObj*)self)->gameScene;
	if (gameScene->loadNavMesh(meshFileName)) {
		float startPos[3]{ 15, 10, -47 };
		float endPos[3]{ 43, 10, -1 };
		std::vector<float> path;
		gameScene->findPath(startPos, endPos, &path);
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

static PyObject* findPath(PyObject* self, PyObject* args) {
	PyObject* spos, *epos;
	if (!PyArg_ParseTuple(args, "OO", &spos, &epos)) {
		Logger::logError("$find path, arg error");
		Py_RETURN_NONE;
	}
	if (!PyTuple_Check(spos) || !PyTuple_Check(epos)) {
		Logger::logError("$find path, pos format error!!");
		Py_RETURN_NONE;
	}
	float startX = PyFloat_AsDouble(PyTuple_GetItem(spos, 0));
	float startY = PyFloat_AsDouble(PyTuple_GetItem(spos, 1));
	float startZ = PyFloat_AsDouble(PyTuple_GetItem(spos, 2));
	float endX = PyFloat_AsDouble(PyTuple_GetItem(epos, 0));
	float endY = PyFloat_AsDouble(PyTuple_GetItem(epos, 1));
	float endZ = PyFloat_AsDouble(PyTuple_GetItem(epos, 2));
	GameScene* gameScene = ((PySceneObj*)self)->gameScene;
	float startPos[3]{ startX, startY, startZ };
	float endPos[3]{ endX, endY, endZ };
	std::vector<float> path;
	gameScene->findPath(startPos, endPos, &path);
	if (path.empty()) {
		Py_RETURN_NONE;
	}

	PyObject* pathTuple = PyTuple_New(path.size() / 3);
	int idx = 0;
	for (auto iter = path.begin(); iter != path.end(); iter += 3) {
		PyObject* pos = PyTuple_New(3);
		for (int i = 0; i < 3; i++) {
			PyTuple_SetItem(pos, i, PyFloat_FromDouble(*(iter + i)));
		}
		PyTuple_SetItem(pathTuple, idx++, pos);
	}
	return pathTuple;
}

static PyMethodDef tp_methods[] = {
	//{"getSceneById", getSceneById, METH_VARARGS, ""},
	{"createPlayer", (PyCFunction)createPlayer, METH_VARARGS, ""},
	{"removeActor", (PyCFunction)removeActor, METH_VARARGS, ""},
	{"onPlayerEnter", (PyCFunction)onPlayerEnter, METH_VARARGS, ""},
	{"loadNavMesh", (PyCFunction)loadNavMesh, METH_VARARGS, "" },
	{"findPath", (PyCFunction)findPath, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};

static PyMemberDef tp_members[] = {
	{"scene_uid", T_OBJECT_EX, offsetof(PySceneObj, scene_uid), 0, "scene uid"},
	{NULL}
};

TYPE_CONSTRUTOR(PyTypeSceneObj) {}
TYPE_MEMBER(PyTypeSceneObj, tp_members)
TYPE_METHOD(PyTypeSceneObj, tp_methods)
TYPE_NEWFUNC(PyTypeSceneObj, PySceneObj_New)
TYPE_FREEFUNC(PyTypeSceneObj, PySceneObj_Free)

