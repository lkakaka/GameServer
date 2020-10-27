#include "PyScene.h"
#include "Logger.h"
#include "PySceneObj.h"
#include "PyGamePlayer.h"

static PyObject* ModuleError;
static const char* ModuleName = "Scene";

static PyObject* getSceneByID(PyObject* self, PyObject* args)
{
	Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
	{"getSceneByID", (PyCFunction)getSceneByID, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python scene interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};

PyMODINIT_FUNC PyInit_Scene(void)
{
	PyObject* module = PyModule_Create(&module_def);
	if (module == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Scene.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(module, "error", ModuleError) < 0) {
		goto error;
	}

	PyTypeSceneObj* sceneObjType = MAKE_PY_OBJ_TYPE(PyTypeSceneObj, ModuleName, "SceneObj", sizeof(PySceneObj));
	if (!sceneObjType->addPyTypeObj(module)) {
		goto error;
	}


	PyTypeGamePlayer* pyGamePlayer = MAKE_PY_OBJ_TYPE(PyTypeGamePlayer, ModuleName, "Player", sizeof(PyGamePlayer));
	if (!pyGamePlayer->addPyTypeObj(module)) {
		goto error;
	}

	return module;

error:
	Py_XDECREF(ModuleError);
	Py_CLEAR(ModuleError);
	Py_DECREF(module);
	return NULL;
}

void initSceneModule() {
	PyImport_AppendInittab(ModuleName, PyInit_Scene);  // python3
}

