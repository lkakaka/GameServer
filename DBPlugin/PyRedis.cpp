#include "PyRedis.h"
#include "Logger.h"

static PyTypeObject PyRedisObj_Type;


static PyObject* PyRedisObj_New(struct _typeobject* tobj, PyObject* args, PyObject* obj2) {
	PyObject* obj = PyType_GenericNew(tobj, args, obj2);
	Redis* redis = new Redis("127.0.0.1", 6379);
	((PyRedisObj*)obj)->redis = redis;
	return obj;
}

static void PyRedisObj_Free(void* ptr) {
	Redis* redis = ((PyRedisObj*)ptr)->redis;
	//printf("PySceneObj_Free-=--\n");
	delete redis;
}


static PyObject* parseRedisReply(redisReply* reply) {
	switch (reply->type) {
		case REDIS_REPLY_INTEGER:
			return PyLong_FromLongLong(reply->integer);
		case REDIS_REPLY_DOUBLE:
			return PyFloat_FromDouble(reply->dval);
		case REDIS_REPLY_NIL:
			Py_RETURN_NONE;
		case REDIS_REPLY_BOOL:
			return PyBool_FromLong(reply->integer);
		case REDIS_REPLY_ATTR:
		case REDIS_REPLY_PUSH:
		case REDIS_REPLY_BIGNUM:
			return PyFloat_FromString(PyUnicode_FromStringAndSize(reply->str, reply->len));
		case REDIS_REPLY_STRING:
		case REDIS_REPLY_ERROR:
		case REDIS_REPLY_STATUS:
			return PyUnicode_FromStringAndSize(reply->str, reply->len);
		case REDIS_REPLY_ARRAY:
		case REDIS_REPLY_MAP:
		case REDIS_REPLY_SET:
		{
			PyObject* array = PyTuple_New(reply->elements);
			for (int i = 0; i < reply->elements; i++) {
				redisReply* subReply = reply->element[i];
				PyTuple_SetItem(array, i, parseRedisReply(subReply));
			}
			return array;
		}
	}
	Logger::logError("not support redis reply type %d", reply->type);
	Py_RETURN_NONE;
}


static PyObject* exeReidsCmd(PyObject* self, PyObject* args) {
	char* redisCmd;
	if (!PyArg_ParseTuple(args, "s", &redisCmd)) {
		//PyErr_SetString(ModuleError, "create scene obj failed");
		Logger::logError("$execute redis cmd failed, arg error");
		Py_RETURN_NONE;
	}
	Redis* redis = ((PyRedisObj*)self)->redis;
	REDIS_REPLY_PTR ptr = redis->execRedisCmd(redisCmd);
	if (ptr == NULL) {
		Py_RETURN_NONE;
	}

	return parseRedisReply(ptr->getReply());
}

static PyMethodDef tp_methods[] = {
	{"exeReidsCmd", (PyCFunction)exeReidsCmd, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};

//static PyMemberDef tp_members[] = {
//	{"scene_uid", T_OBJECT_EX, offsetof(PySceneObj, scene_uid), 0, "scene uid"},
//	{NULL}
//};


static void initPySceneObj_Type()
{
	memset(&PyRedisObj_Type, 0, sizeof(PyRedisObj_Type));
	PyRedisObj_Type.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	PyRedisObj_Type.tp_name = "Scene.SceneObj";
	PyRedisObj_Type.tp_basicsize = sizeof(PyRedisObj);
	PyRedisObj_Type.tp_getattro = PyObject_GenericGetAttr;
	PyRedisObj_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	PyRedisObj_Type.tp_methods = tp_methods;
	//PyRedisObj_Type.tp_members = tp_members;
	PyRedisObj_Type.tp_new = PyRedisObj_New;
	PyRedisObj_Type.tp_free = PyRedisObj_Free;
}

bool addPyRedisObj(PyObject* module) {
	initPySceneObj_Type();
	if (PyType_Ready(&PyRedisObj_Type) < 0) {
		Logger::logError("$add py redis obj error, ready type failed");
		return false;
	}

	Py_INCREF(&PyRedisObj_Type);
	if (PyModule_AddObject(module, "SceneObj", (PyObject*)&PyRedisObj_Type) < 0) {
		Py_DECREF(&PyRedisObj_Type);
		Logger::logError("$add py redis obj error, add failed");
		return false;
	}
	return true;
}
