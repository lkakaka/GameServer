
#include "PyDb.h"
#include "Logger.h"
#include "DBMgr.h"
#include "PyRedis.h"
#include "PyDbInst.h"

static PyObject* ModuleError;
static char* ModuleName = "PyDb";

static PyModuleObj pyModObj;
static PyObject* g_moudle = NULL;

static char* PyStringToString(PyObject* obj) {
	char* str;
	Py_ssize_t len;
	PyObject* bytes = PyUnicode_AsUTF8String(obj);
	PyBytes_AsStringAndSize(bytes, &str, &len);
	return str;
}

static PyObject* createDB(PyObject* self, PyObject* args)
{
	char* dbName = NULL;
	if (!PyArg_ParseTuple(args, "s", &dbName)) {
		PyErr_SetString(ModuleError, "create db handler failed");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = DBMgr::getSingleton()->createDBHander(dbName);
	if (dbHandler == NULL) {
		Py_RETURN_NONE;
	}

	PyObject* tuple = PyTuple_New(1);
	PyTuple_SetItem(tuple, 0, PyLong_FromLong(10));
	//return Py_BuildValue("s", dbHandler->getDbName().c_str());
	return tuple;
}

static PyMethodDef module_methods[] = {
	{"createDB", (PyCFunction)createDB, METH_VARARGS, ""},
	{NULL, NULL, 0, NULL}

};

static struct PyModuleDef module_def =
{
	PyModuleDef_HEAD_INIT,
	ModuleName, /* name of module */
	"python db interface", /* module documentation, may be NULL */
	-1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	module_methods
};


//static PyTypeObject PyDb_Type = {
//	PyVarObject_HEAD_INIT(NULL, 0)
//	"PyDb",                                      /* tp_name */
//	sizeof(PyDbObject), //offsetof(PyLongObject, ob_digit),           /* tp_basicsize */
//	0,//sizeof(digit),                              /* tp_itemsize */
//	0,                                          /* tp_dealloc */
//	0,                                          /* tp_vectorcall_offset */
//	0,                                          /* tp_getattr */
//	0,                                          /* tp_setattr */
//	0,                                          /* tp_as_async */
//	db_repr,                     /* tp_repr */
//	0,                            /* tp_as_number */
//	0,                                          /* tp_as_sequence */
//	0,                                          /* tp_as_mapping */
//	0,                        /* tp_hash */
//	0,                                          /* tp_call */
//	db_repr,                                          /* tp_str */
//	PyDb_GenericGetAttr,                    /* tp_getattro */
//	0,                                          /* tp_setattro */
//	0,                                          /* tp_as_buffer */
//	Py_TPFLAGS_DEFAULT,               /* tp_flags */
//	"",                                   /* tp_doc */
//	0,                                          /* tp_traverse */
//	0,                                          /* tp_clear */
//	0,                           /* tp_richcompare */
//	0,                                          /* tp_weaklistoffset */
//	0,                                          /* tp_iter */
//	0,                                          /* tp_iternext */
//	db_methods,                               /* tp_methods */
//	db_members,                                          /* tp_members */
//	0,                                /* tp_getset */
//	0,                                          /* tp_base */
//	0,                                          /* tp_dict */
//	0,                                          /* tp_descr_get */
//	0,                                          /* tp_descr_set */
//	0,                                          /* tp_dictoffset */
//	0,                                          /* tp_init */
//	0,                                          /* tp_alloc */
//	PyDbInst_New,                                   /* tp_new */
//	PyDbInst_Free,                               /* tp_free */
//};

PyMODINIT_FUNC PyInit_PyDb(void)
{
	g_moudle = PyModule_Create(&module_def);
	if (g_moudle == NULL) {
		LOG_ERROR("init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Db.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(g_moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(g_moudle);
		return NULL;
	}

	PyTypeDbInst* pyDbInst = MAKE_PY_OBJ_TYPE(PyTypeDbInst, ModuleName, "DbInst", sizeof(PyDbObject));
	if (!pyDbInst->addPyTypeObj(g_moudle)) {
		return NULL;
	}
	
	PyTypeRedis* pyRedis = MAKE_PY_OBJ_TYPE(PyTypeRedis, ModuleName, "Redis", sizeof(PyRedisObj));
	if (!pyRedis->addPyTypeObj(g_moudle)) {
		return NULL;
	}

	return g_moudle;
}

void initDbModule() {
	PyImport_AppendInittab(ModuleName, PyInit_PyDb);  // python3
	//PyObject* module = PyImport_ImportModule("PyDb");
}

