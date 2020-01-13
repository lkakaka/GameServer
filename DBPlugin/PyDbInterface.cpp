#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "structmember.h"
#include "PyDbInterface.h"
#include "Logger.h"
#include "DBMgr.h"

#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/datatype.h"

static PyObject* ModuleError;
static char* ModuleName = "PyDb";

typedef struct {
	PyObject_HEAD
	DBHandler* db_inst;
	PyObject* name;
} PyDbObject;

static PyObject* test(PyObject* self, PyObject* args)
{
	PyDbObject* obj = (PyDbObject*)self;
	printf("test exec------");
	Py_RETURN_NONE;
}


static PyObject* db_repr(PyObject* aa)
{
	return Py_BuildValue("s", "PyDb handler, db_repr");
}

static PyObject* createDB(PyObject* self, PyObject* args)
{
	char* dbName = NULL;
	if (!PyArg_ParseTuple(args, "s", &dbName)) {
		PyErr_SetString(ModuleError, "create db handler failed");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = DBMgr::createDBHander(dbName);
	if (dbHandler == NULL) {
		Py_RETURN_NONE;
	}

	PyObject* tuple = PyTuple_New(1);
	PyTuple_SetItem(tuple, 0, PyLong_FromLong(10));
	//return Py_BuildValue("s", dbHandler->getDbName().c_str());
	return tuple;
}

static PyObject* initTable(PyObject* self, PyObject* args)
{
	char* tbName = NULL;
	PyObject* fieldTuple;
	if (!PyArg_ParseTuple(args, "sO", &tbName, &fieldTuple)) {
		PyErr_SetString(ModuleError, "init table failed");
		Py_RETURN_FALSE;
	}
	long length = PyTuple_Size(fieldTuple);
	for (int i = 0; i < length; i++) {
		//get an element out of the list - the element is also a python objects
		PyObject* fieldInfo = PyTuple_GetItem(fieldTuple, i);
		PyObject* val = PyDict_GetItemString(fieldInfo, "fieldName");
		PyObject* bytes;
		char* fieldName;
		Py_ssize_t len;
		bytes = PyUnicode_AsUTF8String(val);
		PyBytes_AsStringAndSize(bytes, &fieldName, &len);
		//char* fieldName = PyBytes_AsString(val);
		val = PyDict_GetItemString(fieldInfo, "filedType");
		int fieldType = PyLong_AsLong(val);
		Logger::logInfo("$init table %s, field:%s, fieldType:%d", tbName, fieldName, fieldType);
	}
	Py_RETURN_TRUE;
}

static PyObject* createTable(PyObject* self, PyObject* args)
{
	char* hander;
	PyObject* tb;
	if (!PyArg_ParseTuple(args, "sO", &hander, &tb)) {
		PyErr_SetString(ModuleError, "execute sql failed");
		Py_RETURN_FALSE;
	}
	PyObject* dictObj = PyObject_GenericGetDict(tb, NULL);
	Py_ssize_t pos = 0;
	PyObject* key;
	PyObject* val;
	while (PyDict_Next(dictObj, &pos, &key, &val)) {
		//PyBytes_AsString(val);
		char* keyName;
		PyObject* bytes;
		Py_ssize_t len;
		bytes = PyUnicode_AsUTF8String(key);
		PyBytes_AsStringAndSize(bytes, &keyName, &len);
		Logger::logInfo("$create table, field:%s", keyName);
	}

	PyObject* ageObj = PyObject_GetAttrString(tb, "age");
	int age = PyLong_AsLong(ageObj);
	Py_RETURN_TRUE;
}

static PyObject* executeSql(PyObject* self, PyObject* args)
{
	char* sql = NULL;
	if (!PyArg_ParseTuple(args, "s", &sql)) {
		Logger::logError("$execute sql failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		PyErr_SetString(ModuleError, "execute sql failed, db hander is null");
		Py_RETURN_NONE;
	}

	PyObject* result = PyList_New(0);
	auto dataHandler = [&result](sql::Statement* st, bool isResultSet) {
		/*Logger::logInfo("$execute sql return!!");*/
		while (true) {
			if (isResultSet) {
				sql::ResultSet* rs = st->getResultSet();
				sql::ResultSetMetaData* metaData = rs->getMetaData();
				int colCount = metaData->getColumnCount();
				PyObject* fieldTuple = PyTuple_New(colCount);
				for (int i = 1; i <= colCount; i++) {
					//std::string fieldName = metaData->getColumnLabel(i).c_str();
					/*const char* name = metaData->getColumnLabel(i).c_str();*/
					PyTuple_SetItem(fieldTuple, i - 1, PyUnicode_FromString(metaData->getColumnLabel(i).c_str()));
				}
				PyList_Append(result, fieldTuple);

				while (rs->next()) {
					PyObject* dataTuple = PyTuple_New(metaData->getColumnCount());
					for (int i = 1; i <= colCount; i++) {
						int colType = metaData->getColumnType(i);
						if (colType >= sql::DataType::BIT && colType <= sql::DataType::BIGINT) {
							PyTuple_SetItem(dataTuple, i - 1, PyLong_FromLong(rs->getInt(i)));
						}
						else if (colType >= sql::DataType::REAL && colType <= sql::DataType::NUMERIC) {
							PyTuple_SetItem(dataTuple, i - 1, PyFloat_FromDouble(rs->getDouble(i)));
						}
						else {
							PyTuple_SetItem(dataTuple, i - 1, PyUnicode_FromString(rs->getString(i).c_str()));
						}
					}
					PyList_Append(result, dataTuple);
				}
			}
			else {
				int updateCount = st->getUpdateCount();
				if (updateCount < 0) {
					break;
				}
				PyObject* filedTuple = PyTuple_New(1);
				PyTuple_SetItem(filedTuple, 0, PyUnicode_FromString("update_count"));
				PyList_Append(result, filedTuple);
				PyObject* dataTuple = PyTuple_New(1);
				PyTuple_SetItem(dataTuple, 0, PyLong_FromLong(updateCount));
				PyList_Append(result, dataTuple);
			}

			isResultSet = st->getMoreResults();
		}
	};

	try {
		dbHandler->executeSql(sql, dataHandler);
	}
	catch (std::exception& e) {
		Logger::logError("$execute sql failed, sql:%s, ex: %s", sql, e.what());
		Py_RETURN_NONE;
	}
	return result;
}

static PyMethodDef module_methods[] = {
	{"createDB", (PyCFunction)createDB, METH_VARARGS, ""},
	{"initTable", (PyCFunction)initTable, METH_VARARGS, ""},
	{"createTable", (PyCFunction)createTable, METH_VARARGS, ""},
	//{"executeSql", (PyCFunction)executeSql, METH_VARARGS, ""},
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


static PyMethodDef db_methods[] = {
	{"test", test, METH_NOARGS, "test"},
	{"executeSql", executeSql, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};

static PyMemberDef db_members[] = {
	{"name", T_OBJECT_EX, offsetof(PyDbObject, name), 0, "first name"},
	{NULL}
};


static PyObject* PyDbInst_New(struct _typeobject* tobj,
	PyObject* args, PyObject* obj2) {
	char* dbName = NULL;
	if (!PyArg_ParseTuple(args, "s", &dbName)) {
		//PyErr_SetString(ModuleError, "create db inst failed");
		Logger::logInfo("$create db inst failed, arg error");
		return NULL;
	}
	DBHandler* dbHandler = DBMgr::createDBHander(dbName);
	if (dbHandler == NULL) {
		Logger::logInfo("$create db inst failed, db handler exist, dbName:%s", dbName);
		return NULL;
	}
	//printf("PyDbInst_New, %s\n", dbName);
	PyObject* dbInst = PyType_GenericNew(tobj, args, obj2);
	((PyDbObject*)dbInst)->db_inst = dbHandler;
	((PyDbObject*)dbInst)->name = PyUnicode_FromString(dbName);
	return dbInst;
}

static void PyDbInst_Free(void* ptr) {
	//printf("PyDbInst_Free-=--\n");
	PyObject_Del(ptr);
}

static PyObject*
PyDb_GenericGetAttr(PyObject* obj, PyObject* name) {
	//PyObject* bytes = PyBytes_FromObject(name);
	const char* namStr = PyUnicode_AsUTF8(name);
	//printf("PyDb_GenericGetAttR------%s\n", namStr);
	return PyObject_GenericGetAttr(obj, name);
}

static PyTypeObject PyDb_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"PyDb",                                      /* tp_name */
	sizeof(PyDbObject), //offsetof(PyLongObject, ob_digit),           /* tp_basicsize */
	0,//sizeof(digit),                              /* tp_itemsize */
	0,                                          /* tp_dealloc */
	0,                                          /* tp_vectorcall_offset */
	0,                                          /* tp_getattr */
	0,                                          /* tp_setattr */
	0,                                          /* tp_as_async */
	db_repr,                     /* tp_repr */
	0,                            /* tp_as_number */
	0,                                          /* tp_as_sequence */
	0,                                          /* tp_as_mapping */
	0,                        /* tp_hash */
	0,                                          /* tp_call */
	db_repr,                                          /* tp_str */
	PyDb_GenericGetAttr,                    /* tp_getattro */
	0,                                          /* tp_setattro */
	0,                                          /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,               /* tp_flags */
	"",                                   /* tp_doc */
	0,                                          /* tp_traverse */
	0,                                          /* tp_clear */
	0,                           /* tp_richcompare */
	0,                                          /* tp_weaklistoffset */
	0,                                          /* tp_iter */
	0,                                          /* tp_iternext */
	db_methods,                               /* tp_methods */
	db_members,                                          /* tp_members */
	0,                                /* tp_getset */
	0,                                          /* tp_base */
	0,                                          /* tp_dict */
	0,                                          /* tp_descr_get */
	0,                                          /* tp_descr_set */
	0,                                          /* tp_dictoffset */
	0,                                          /* tp_init */
	0,                                          /* tp_alloc */
	PyDbInst_New,                                   /* tp_new */
	PyDbInst_Free,                               /* tp_free */
};


PyMODINIT_FUNC PyInit_PyDb(void)
{
	if (PyType_Ready(&PyDb_Type) < 0) {
		return NULL;
	}
	
	PyObject* moudle = PyModule_Create(&module_def);
	if (moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
		return NULL;
	}

	ModuleError = PyErr_NewException("Db.error", NULL, NULL);
	Py_XINCREF(ModuleError);
	if (PyModule_AddObject(moudle, "error", ModuleError) < 0) {
		Py_XDECREF(ModuleError);
		Py_CLEAR(ModuleError);
		Py_DECREF(moudle);
		return NULL;
	}

	Py_INCREF(&PyDb_Type);
	//PyObject* obj = PyObject_New(PyObject, &PyDb_Type);;
	if (PyModule_AddObject(moudle, "DbInst", (PyObject*)&PyDb_Type) < 0){ 
		Py_DECREF(&PyDb_Type);
		Py_DECREF(moudle);
		return NULL;
	}

	return moudle;
}

void initDbModule() {
	PyImport_AppendInittab(ModuleName, PyInit_PyDb);  // python3
	Logger::initLog();
}

