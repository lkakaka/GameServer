#define PY_SSIZE_T_CLEAN

#include "Python.h"
#include "structmember.h"
#include "PyDbInterface.h"
#include "Logger.h"
#include "DBMgr.h"

#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/datatype.h"

#include "PyCustomObj.h"

#include "../Common/PyCommon.h"
#include "PyRedis.h"

static PyObject* ModuleError;
static char* ModuleName = "PyDb";

//class PyCustomObjBase {
//public:
//	PyObject_HEAD
//};

class PyDbObject : public PyCustomObjBase {
public:
	//PyObject_HEAD
	DBHandler* db_inst;
	PyObject* name;
};

//class PyModuleObj {
//	PyTypeObject  pyTypeObj;
//	int custom_type_size;
//
//};

static char* PyStringToString(PyObject* obj) {
	char* str;
	Py_ssize_t len;
	PyObject* bytes = PyUnicode_AsUTF8String(obj);
	PyBytes_AsStringAndSize(bytes, &str, &len);
	return str;
}

static PyObject* test(PyObject* self, PyObject* args)
{
	PyDbObject* obj = (PyDbObject*)self;
	printf("test exec------");
	Py_RETURN_NONE;
}


static PyObject* db_repr(PyObject* self)
{
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	char buf[64]{ 0 };
	snprintf(buf, 64, "<C++ Object DBHandler:%lld>", (long long)dbHandler);
	return Py_BuildValue("s", buf);
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

static bool _initTable(DBHandler* dbHandler, PyObject* tblObj) {
	Table tbl;
	char* tbName = PyStringToString(PyObject_GetAttrString(tblObj, "tb_name"));
	tbl.tableName = tbName;
	PyObject * colTuple = PyObject_GetAttrString(tblObj, "_columns");
	ssize_t colNum =  PyTuple_Size(colTuple);
	bool hasKey = false;
	for (int col = 0; col < colNum; col++) {
		PyObject* colObj = PyTuple_GetItem(colTuple, col);
		char* colName = PyStringToString(PyObject_GetAttrString(colObj, "name"));
		PyObject* defaultObj = PyObject_GetAttrString(colObj, "default");
		long type = PyLong_AsLong(PyObject_GetAttrString(colObj, "type"));
		
		MAKE_TABLE_FIELD(field);
		field->fieldName = colName;
		field->type = (TableField::FieldType)type;
		field->oldName = PyStringToString(PyObject_GetAttrString(colObj, "old_name"));
		field->isDel = PyLong_AsLong(PyObject_GetAttrString(colObj, "is_del")) == 1;
		switch (type)
		{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				if (defaultObj != Py_None) {
					long defVal = PyLong_AsLong(defaultObj);
					field->defaut_val = defVal;
				}
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				if (defaultObj != Py_None) {
					long defVal = PyFloat_AsDouble(defaultObj);
					field->defaut_val = defVal;
				}
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			{
				PyObject* lenObj = PyObject_GetAttrString(colObj, "length");
				if (lenObj != Py_None) {
					long len = PyLong_AsLong(lenObj);
					field->length = len;
				}
				if (defaultObj != Py_None) {
					char* defVal = PyStringToString(defaultObj);
					if (strcmp(defVal, "") != 0) {
						field->defaut_val.append("'").append(defVal).append("'");
					}
				}
				break;
			}
			case TableField::FieldType::TYPE_TEXT:
			{
				if (defaultObj != Py_None) {
					char* defVal = PyStringToString(defaultObj);
					field->defaut_val.append("'").append(defVal).append("'");
				}
				break;
			}
			default:
				Logger::logError("$not support table col type %ld, table:%s", type, tbName);
				return false;
		}
		PyObject* keyObj = PyObject_GetAttrString(colObj, "key");
		if (keyObj != Py_None && PyLong_AsLong(keyObj) == 1) {
			if (hasKey) {
				Logger::logError("$creat table %s failed, has set primary key", tbName);
				return false;
			}
			tbl.priKeyName = colName;
			hasKey = true;
		}

		tbl.addField(field);
	}

	if (!hasKey) {
		Logger::logError("$creat table %s failed, not set primary key", tbName);
		return false;
	}
	
	// ³õÊ¼»¯Ë÷Òý
	if (PyObject_HasAttrString(tblObj, "_indexs")) {
		PyObject* tbIndexTuple = PyObject_GetAttrString(tblObj, "_indexs");
		if (tbIndexTuple != NULL && tbIndexTuple != Py_None) {
			ssize_t indexNum = PyTuple_Size(tbIndexTuple);
			for (int idx = 0; idx < indexNum; idx++) {
				TableIndex tblIndex;
				PyObject* idxObj = PyTuple_GetItem(tbIndexTuple, idx);
				PyObject* closObj = PyObject_GetAttrString(idxObj, "cols");
				tblIndex.isUnique = PyLong_AsLong(PyObject_GetAttrString(idxObj, "is_unique")) == 1;
				ssize_t idxColNum = PyTuple_Size(closObj);
				for (int idxCol = 0; idxCol < idxColNum; idxCol++) {
					PyObject* colNameObj = PyTuple_GetItem(closObj, idxCol);
					tblIndex.cols.emplace_back(PyStringToString(colNameObj));
				}
				tbl.tableIndexs.emplace_back(tblIndex);
			}
		}
	}

	return dbHandler->createTable(&tbl);
}

static PyObject* initTable(PyObject* self, PyObject* args)
{
	//char* tbName = NULL;
	//PyObject* fieldTuple;
	//if (!PyArg_ParseTuple(args, "sO", &tbName, &fieldTuple)) {
	//	PyErr_SetString(ModuleError, "init table failed");
	//	Py_RETURN_FALSE;
	//}
	//ssize_t length = PyTuple_Size(fieldTuple);
	//for (int i = 0; i < length; i++) {
	//	//get an element out of the list - the element is also a python objects
	//	PyObject* fieldInfo = PyTuple_GetItem(fieldTuple, i);
	//	PyObject* val = PyDict_GetItemString(fieldInfo, "fieldName");
	//	PyObject* bytes;
	//	char* fieldName;
	//	Py_ssize_t len;
	//	bytes = PyUnicode_AsUTF8String(val);
	//	PyBytes_AsStringAndSize(bytes, &fieldName, &len);
	//	//char* fieldName = PyBytes_AsString(val);
	//	val = PyDict_GetItemString(fieldInfo, "filedType");
	//	int fieldType = PyLong_AsLong(val);
	//	Logger::logInfo("$init table %s, field:%s, fieldType:%d", tbName, fieldName, fieldType);
	//}
	//Py_RETURN_TRUE;

	PyObject* tblTuple;
	if (!PyArg_ParseTuple(args, "O", &tblTuple)) {
		PyErr_SetString(ModuleError, "init table failed");
		Py_RETURN_FALSE;
	}

	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		PyErr_SetString(ModuleError, "init tables failed, db hander is null");
		Py_RETURN_FALSE;
	}

	if (!PyObject_TypeCheck(tblTuple, &PyTuple_Type)) {
		Logger::logError("$init table failed, args format error");
		Py_RETURN_FALSE;
	}

	ssize_t size = PyTuple_Size(tblTuple);
	for (int i = 0; i < size; i++) {
		PyObject* tbObj = PyTuple_GetItem(tblTuple, i);
		if (!_initTable(dbHandler, tbObj)) {
			char* tbName = PyStringToString(PyObject_GetAttrString(tbObj, "tb_name"));
			Logger::logError("$table %s init failed", tbName);
			Py_RETURN_FALSE;
		}
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

	StatementPtr ptr = dbHandler->executeSql(sql);
	if (ptr == NULL) Py_RETURN_NONE;
	sql::Statement* st = ptr->getStatement();
	bool isResultSet = ptr->isResultSet();

	PyObject* result = PyList_New(0);
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
			int64_t updateCount = st->getUpdateCount();
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

	return result;
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

static PyObject* insertRow(PyObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj)) {
		Logger::logError("$insert row failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		PyErr_SetString(ModuleError, "insert row failed, db hander is null");
		Py_RETURN_NONE;
	}

	PyObject* tableNameObj = PyObject_GetAttrString(obj, "table_name");
	PyObject* priKeyObj = PyObject_GetAttrString(obj, "pri_key");
	PyObject* fields = PyObject_GetAttrString(obj, "fields");

	char* tableName = PyStringToString(tableNameObj);
	long priKeyVal = PyLong_AsLong(priKeyObj);

	Table tbl;
	tbl.tableName = tableName;
	tbl.priKeyVal = priKeyVal;
	Py_ssize_t pos = 0;
	PyObject* key;
	PyObject* val;
	while (PyDict_Next(fields, &pos, &key, &val)) {
		char* fieldName = PyStringToString(key);
		DataBase::TableField* fieldDesc = dbHandler->getTableField(tableName, fieldName);
		if (fieldDesc == NULL) {
			Py_RETURN_NONE;
		}

		MAKE_TABLE_FIELD(field);
		field->fieldName = fieldName;
		field->type = fieldDesc->type;
		switch (fieldDesc->type)
		{
		case TableField::FieldType::TYPE_INT:
		case TableField::FieldType::TYPE_BIGINT:
		{
			field->lval = PyLong_AsLong(val);
			break;
		}
		case TableField::FieldType::TYPE_DOUBLE:
		{
			field->dval = PyFloat_AsDouble(val);
			break;
		}
		case TableField::FieldType::TYPE_VCHAR:
		case TableField::FieldType::TYPE_TEXT:
		{
			field->sval = PyStringToString(val);
			break;
		}
		default:
		{
			Logger::logError("$not support field type, table:%s, field:%s", tableName, fieldName);
			Py_RETURN_NONE;
		}
		}
		tbl.addField(field);
	}

	if (!dbHandler->insertRow(&tbl))
	{
		Py_RETURN_NONE;
	}
	return PyLong_FromLong(tbl.priKeyVal);
}

static void PyTableToTable(PyObject* pyTbl, Table* tbl) {
	PyObject* tableNameObj = PyObject_GetAttrString(pyTbl, "tb_name");
	tbl->tableName = PyStringToString(tableNameObj);

	PyObject* colTuple = PyObject_GetAttrString(pyTbl, "_columns");
	ssize_t colNum = PyTuple_Size(colTuple);
	for (int col = 0; col < colNum; col++) {
		PyObject* colObj = PyTuple_GetItem(colTuple, col);
		char* colName = PyStringToString(PyObject_GetAttrString(colObj, "name"));
		long type = PyLong_AsLong(PyObject_GetAttrString(colObj, "type"));
		if (PyObject_HasAttrString(pyTbl, colName)) {
			PyObject* colObj = PyObject_GetAttrString(pyTbl, colName);
			if (colObj == Py_None) {
				continue;
			}
			MAKE_TABLE_FIELD(tbField);
			tbField->fieldName = colName;
			tbField->type = (TableField::FieldType)type;
			switch (type)
			{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				tbField->lval = PyLong_AsLong(colObj);
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				tbField->defaut_val = PyFloat_AsDouble(colObj);
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				tbField->sval = PyStringToString(colObj);
				break;
			}
			default:
				Logger::logError("$not support table col type %ld, table:%s", type, tbl->tableName.c_str());
			}
			tbl->addField(tbField);
		}
	}
}

static PyObject* TableToPyTable(Table* tbl) {
	std::string tableName = tbl->tableName;
	PyObject* args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, PyUnicode_FromString(tableName.c_str()));
	PyObject* tblObj = callPyFunction("main", "create_tb", args);
	PyObject* colTuple = PyObject_GetAttrString(tblObj, "_columns");
	ssize_t colNum = PyTuple_Size(colTuple);
	for (int col = 0; col < colNum; col++) {
		PyObject* colObj = PyTuple_GetItem(colTuple, col);
		char* colName = PyStringToString(PyObject_GetAttrString(colObj, "name"));
		TableField* field = tbl->getField(colName);
		if (field == NULL) {
			continue;
		}
		long type = PyLong_AsLong(PyObject_GetAttrString(colObj, "type"));
		switch (type)
		{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				PyObject_SetAttrString(tblObj, colName, PyLong_FromLong(field->lval));
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				PyObject_SetAttrString(tblObj, colName, PyFloat_FromDouble(field->dval));
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				PyObject_SetAttrString(tblObj, colName, PyUnicode_FromString(field->sval.c_str()));
				break;
			}
			default:
				Logger::logError("$not support table col type %ld, table:%s", type, tbl->tableName.c_str());
		}
	}
	return tblObj;
}

static PyObject* getRow(PyObject* self, PyObject* args)
{
	PyObject* tblObj;
	if (!PyArg_ParseTuple(args, "O", &tblObj)) {
		Logger::logError("$get row failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		PyErr_SetString(ModuleError, "get row failed, db hander is null");
		Py_RETURN_NONE;
	}

	Table tbl;
	PyTableToTable(tblObj, &tbl);
	std::vector<Table> result;
	if (!dbHandler->getRow(&tbl, result)) {
		Py_RETURN_NONE;
	}

	PyObject* lst = PyList_New(0);
	for (Table tbl : result) {
		PyObject* obj = TableToPyTable(&tbl);
		PyList_Append(lst, obj);
	}

	return lst;
}

static PyObject* updateRow(PyObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj)) {
		Logger::logError("$update row failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		PyErr_SetString(ModuleError, "update row failed, db hander is null");
		Py_RETURN_NONE;
	}

	PyObject* tableNameObj = PyObject_GetAttrString(obj, "table_name");
	PyObject* priKeyObj = PyObject_GetAttrString(obj, "pri_key");
	PyObject* fields = PyObject_GetAttrString(obj, "fields");

	char* tableName = PyStringToString(tableNameObj);
	long priKeyVal = PyLong_AsLong(priKeyObj);

	Table tbl;
	tbl.tableName = tableName;
	tbl.priKeyVal = priKeyVal;
	Py_ssize_t pos = 0;
	PyObject* key;
	PyObject* val;
	while (PyDict_Next(fields, &pos, &key, &val)) {
		char* fieldName = PyStringToString(key);
		DataBase::TableField* fieldDesc = dbHandler->getTableField(tableName, fieldName);
		if (fieldDesc == NULL) {
			Py_RETURN_NONE;
		}
		
		MAKE_TABLE_FIELD(field);
		field->fieldName = fieldName;
		field->type = fieldDesc->type;
		switch (fieldDesc->type)
		{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				field->lval = PyLong_AsLong(val);
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				field->dval = PyFloat_AsDouble(val);
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				field->sval = PyStringToString(val);
				break;
			}
			default:
			{	
				Logger::logError("$not support field type, table:%s, field:%s", tableName, fieldName);
				Py_RETURN_NONE;
			}
		}
		tbl.addField(field);
	}

	if (!dbHandler->updateRow(&tbl))
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject* deleteRow(PyObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj)) {
		Logger::logError("$delete row failed, args is error!!");
		Py_RETURN_FALSE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		PyErr_SetString(ModuleError, "delete row failed, db hander is null");
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
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
	{"initTable", initTable, METH_VARARGS, ""},
	{"insertRow", insertRow, METH_VARARGS, ""},
	{"getRow", getRow, METH_VARARGS, ""},
	{"updateRow", updateRow, METH_VARARGS, ""},
	{"deleteRow", deleteRow, METH_VARARGS, ""},
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

static PyTypeObject PyDb_Type;


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

static void initPyDb_Type()
{
	memset(&PyDb_Type, 0, sizeof(PyDb_Type));
	PyDb_Type.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	PyDb_Type.tp_name = "PyDb";
	PyDb_Type.tp_basicsize = sizeof(PyDbObject);
	PyDb_Type.tp_repr = db_repr;
	PyDb_Type.tp_getattro = PyDb_GenericGetAttr;
	PyDb_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	PyDb_Type.tp_methods = db_methods;
	PyDb_Type.tp_members = db_members;
	PyDb_Type.tp_new = PyDbInst_New;
	PyDb_Type.tp_free = PyDbInst_Free;
}

static PyModuleObj pyModObj;

static PyObject* g_moudle = NULL;

PyMODINIT_FUNC PyInit_PyDb(void)
{
	/*initPyDb_Type();
	if (PyType_Ready(&PyDb_Type) < 0) {
		return NULL;
	}*/
	
	g_moudle = PyModule_Create(&module_def);
	if (g_moudle == NULL) {
		Logger::logInfo("$init module %s failed", module_def.m_name);
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

	//Py_INCREF(&PyDb_Type);
	////PyObject* obj = PyObject_New(PyObject, &PyDb_Type);;
	//if (PyModule_AddObject(moudle, "DbInst", (PyObject*)&PyDb_Type) < 0){ 
	//	Py_DECREF(&PyDb_Type);
	//	Py_DECREF(moudle);
	//	return NULL;
	//}

	//initPyDb_Type();
	//PyModuleObj* moduleObj = new PyModuleObj();
	//pyModObj.pyTypeObj = &PyDb_Type;

	//addModuleObj(moudle, "DbInst", pyModObj);
	pyModObj.obj_name = "DbInst";
	pyModObj.tp_methods = db_methods;
	pyModObj.tp_members = db_members;
	pyModObj.tp_new = PyDbInst_New;
	pyModObj.tp_free = PyDbInst_Free;
	pyModObj.tp_repr = db_repr;
	pyModObj.tp_basicsize = sizeof(PyDbObject);
	pyModObj.tp_name = "PyDb.DbInst";
	pyModObj.addToModule(g_moudle);

	if(!addPyRedisObj(g_moudle)) { 
		return NULL;
	}

	return g_moudle;
}

void initDbModule() {
	PyImport_AppendInittab(ModuleName, PyInit_PyDb);  // python3
	//PyObject* module = PyImport_ImportModule("PyDb");
}

