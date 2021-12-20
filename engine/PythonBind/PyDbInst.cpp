#include "PyDbInst.h"
#include "Logger.h"
#include "DBMgr.h"


static PyObject* PyDbInst_New(struct _typeobject* tobj,
	PyObject* args, PyObject* obj2) {
	char* dbName = NULL;
	if (!PyArg_ParseTuple(args, "s", &dbName)) {
		//PyErr_SetString(ModuleError, "create db inst failed");
		LOG_ERROR("create db inst failed, arg error");
		return NULL;
	}
	DBHandler* dbHandler = DBMgr::getSingleton()->createDBHander(dbName);
	if (dbHandler == NULL) {
		LOG_ERROR("create db inst failed, db handler exist, dbName:%s", dbName);
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

static PyObject* db_repr(PyObject* self)
{
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	char buf[64]{ 0 };
	snprintf(buf, 64, "<C++ Object DBHandler:%lld>", (long long)dbHandler);
	return Py_BuildValue("s", buf);
}

static PyObject* executeSql(PyObject* self, PyObject* args)
{
	char* sql = NULL;
	if (!PyArg_ParseTuple(args, "s", &sql)) {
		LOG_ERROR("execute sql failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		LOG_ERROR("execute sql failed, db hander is null!!");
		Py_RETURN_NONE;
	}

	StatementPtr ptr = dbHandler->executeSql(sql);
	if (ptr == NULL) Py_RETURN_NONE;
	//sql::Statement* st = ptr->getStatement();
	bool isResultSet = ptr->isResultSet();

	PyObject* result = PyList_New(0);
	/*LOG_INFO("execute sql return!!");*/
	while (true) {
		if (isResultSet) {
			sql::ResultSet* rs = ptr->getResultSet();
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
			int64_t updateCount = ptr->getUpdateCount();
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

		isResultSet = ptr->getMoreResults();
	}

	return result;
}

static bool _initTable(DBHandler* dbHandler, PyObject* tblObj) {
	Table tbl;
	const char* tbName = PyUnicode_AsUTF8(PyObject_GetAttrString(tblObj, "tb_name"));
	tbl.tableName = tbName;
	PyObject* colTuple = PyObject_GetAttrString(tblObj, "_columns");
	ssize_t colNum = PyTuple_Size(colTuple);
	bool hasKey = false;
	for (int col = 0; col < colNum; col++) {
		PyObject* colObj = PyTuple_GetItem(colTuple, col);
		const char* colName = PyUnicode_AsUTF8(PyObject_GetAttrString(colObj, "name"));
		PyObject* defaultObj = PyObject_GetAttrString(colObj, "default");
		long type = PyLong_AsLong(PyObject_GetAttrString(colObj, "type"));

		MAKE_TABLE_FIELD(field);
		field->fieldName = colName;
		field->type = (TableField::FieldType)type;
		field->oldName = PyUnicode_AsUTF8(PyObject_GetAttrString(colObj, "old_name"));
		field->isDel = PyLong_AsLong(PyObject_GetAttrString(colObj, "is_del")) == 1;
		switch (type)
		{
		case TableField::FieldType::TYPE_INT:
		case TableField::FieldType::TYPE_BIGINT:
		{
			if (defaultObj != Py_None) {
				int64_t defVal = PyLong_AsLongLong(defaultObj);
				field->defaut_val = defVal;
			}
			break;
		}
		case TableField::FieldType::TYPE_DOUBLE:
		{
			if (defaultObj != Py_None) {
				double defVal = PyFloat_AsDouble(defaultObj);
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
				const char* defVal = PyUnicode_AsUTF8(defaultObj);
				if (strcmp(defVal, "") != 0) {
					field->defaut_val.append("'").append(defVal).append("'");
				}
			}
			break;
		}
		case TableField::FieldType::TYPE_TEXT:
		{
			if (defaultObj != Py_None) {
				const char* defVal = PyUnicode_AsUTF8(defaultObj);
				field->defaut_val.append("'").append(defVal).append("'");
			}
			break;
		}
		default:
			LOG_ERROR("not support table col type %ld, table:%s", type, tbName);
			return false;
		}
		PyObject* keyObj = PyObject_GetAttrString(colObj, "key");
		if (keyObj != Py_None && PyLong_AsLong(keyObj) == 1) {
			if (hasKey) {
				LOG_ERROR("creat table %s failed, has set primary key", tbName);
				return false;
			}
			tbl.priKeyName = colName;
			PyObject* autoIncrObj = PyObject_GetAttrString(colObj, "auto_incr");
			if (PyLong_AsLong(autoIncrObj) == 1) {
				tbl.isAutoIncr = true;
			}
			hasKey = true;
		}

		tbl.addField(field);
	}

	if (!hasKey) {
		LOG_ERROR("creat table %s failed, not set primary key", tbName);
		return false;
	}

	// 初始化索引
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
					tblIndex.cols.emplace_back(PyUnicode_AsUTF8(colNameObj));
				}
				tbl.tableIndexs.emplace_back(tblIndex);
			}
		}
	}

	return dbHandler->initTable(&tbl);
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
	//	LOG_INFO("init table %s, field:%s, fieldType:%d", tbName, fieldName, fieldType);
	//}
	//Py_RETURN_TRUE;

	PyObject* tblTuple;
	if (!PyArg_ParseTuple(args, "O", &tblTuple)) {
		LOG_ERROR("init table failed!!");
		Py_RETURN_FALSE;
	}

	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		LOG_ERROR("init tables failed, db hander is null!");
		Py_RETURN_FALSE;
	}

	if (!PyObject_TypeCheck(tblTuple, &PyTuple_Type)) {
		LOG_ERROR("init table failed, args format error");
		Py_RETURN_FALSE;
	}

	ssize_t size = PyTuple_Size(tblTuple);
	for (int i = 0; i < size; i++) {
		PyObject* tbObj = PyTuple_GetItem(tblTuple, i);
		if (!_initTable(dbHandler, tbObj)) {
			const char* tbName = PyUnicode_AsUTF8(PyObject_GetAttrString(tbObj, "tb_name"));
			LOG_ERROR("table %s init failed", tbName);
			Py_RETURN_FALSE;
		}
	}
	dbHandler->initTableSchema();
	Py_RETURN_TRUE;
}

static PyObject* insertRow(PyObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj)) {
		LOG_ERROR("insert row failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		LOG_ERROR("insert row failed, db hander is null!!");
		Py_RETURN_NONE;
	}

	PyObject* tableNameObj = PyObject_GetAttrString(obj, "table_name");
	PyObject* priKeyObj = PyObject_GetAttrString(obj, "pri_key");
	PyObject* fields = PyObject_GetAttrString(obj, "fields");

	const char* tableName = PyUnicode_AsUTF8(tableNameObj);
	long priKeyVal = PyLong_AsLong(priKeyObj);

	Table tbl;
	tbl.tableName = tableName;
	tbl.priKeyVal = priKeyVal;
	Py_ssize_t pos = 0;
	PyObject* key;
	PyObject* val;
	while (PyDict_Next(fields, &pos, &key, &val)) {
		const char* fieldName = PyUnicode_AsUTF8(key);
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
			field->lval = PyLong_AsLongLong(val);
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
			field->sval = PyUnicode_AsUTF8(val);
			break;
		}
		default:
		{
			LOG_ERROR("not support field type, table:%s, field:%s", tableName, fieldName);
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
	//PyObject* tableNameObj = PyObject_GenericGetAttr(pyTbl, PyUnicode_FromString("tb_name"));
	PyObject* tableNameObj = PyObject_GetAttrString(pyTbl, "tb_name");
	tbl->tableName = PyUnicode_AsUTF8(tableNameObj);

	PyObject* colTuple = PyObject_GetAttrString(pyTbl, "_columns");
	ssize_t colNum = PyTuple_Size(colTuple);
	for (int col = 0; col < colNum; col++) {
		PyObject* colObj = PyTuple_GetItem(colTuple, col);
		const char* colName = PyUnicode_AsUTF8(PyObject_GetAttrString(colObj, "name"));
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
				tbField->lval = PyLong_AsLongLong(colObj);
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
				tbField->sval = PyUnicode_AsUTF8(colObj);
				break;
			}
			default:
				LOG_ERROR("not support table col type %ld, table:%s", type, tbl->tableName.c_str());
			}
			tbl->addField(tbField);
		}
	}
}

static PyObject* TableToPyTable(Table* tbl) {
	std::string tableName = tbl->tableName;
	PyObject* args = PyTuple_New(1);
	PyTuple_SetItem(args, 0, PyUnicode_FromString(tableName.c_str()));
	/*PyObject* tblObj = callPyFunction("main", "create_tb", args);*/

	std::string s = "game.db.tbl.tbl_" + tableName;
	PyObject* pModule = PyImport_ImportModule(s.c_str());//这里是要调用的文件名
	int x = toupper(tableName[0]);
	std::string clsName = "Tbl";
	clsName.push_back(toupper(tableName[0]));
	clsName += tableName.substr(1, -1);
	PyObject* pFunc = PyObject_GetAttrString(pModule, clsName.c_str());//这里是要调用的函数名
	PyObject* tblObj = PyEval_CallObject(pFunc, NULL);//调用函数

	PyObject* colTuple = PyObject_GetAttrString(tblObj, "_columns");
	ssize_t colNum = PyTuple_Size(colTuple);
	for (int col = 0; col < colNum; col++) {
		PyObject* colObj = PyTuple_GetItem(colTuple, col);
		const char* colName = PyUnicode_AsUTF8(PyObject_GetAttrString(colObj, "name"));
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
			PyObject_SetAttrString(tblObj, colName, PyLong_FromLongLong(field->lval));
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
			LOG_ERROR("not support table col type %ld, table:%s", type, tbl->tableName.c_str());
		}
	}
	return tblObj;
}

static PyObject* getRow(PyObject* self, PyObject* args)
{
	PyObject* tblObj;
	if (!PyArg_ParseTuple(args, "O", &tblObj)) {
		LOG_ERROR("get row failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		LOG_ERROR("get row failed, db hander is null!!");
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
		LOG_ERROR("update row failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		LOG_ERROR("update row failed, db hander is null!!");
		Py_RETURN_NONE;
	}

	Table tbl;
	PyTableToTable(obj, &tbl);

	/*PyObject* tableNameObj = PyObject_GetAttrString(obj, "table_name");
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
				field->lval = PyLong_AsLongLong(val);
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
				LOG_ERROR("not support field type, table:%s, field:%s", tableName, fieldName);
				Py_RETURN_NONE;
			}
		}
		tbl.addField(field);
	}*/

	if (!dbHandler->updateRow(&tbl))
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject* replaceRows(PyObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj)) {
		LOG_ERROR("replace row failed, args is error!!");
		Py_RETURN_NONE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		LOG_ERROR("replace row failed, db hander is null!!");
		Py_RETURN_NONE;
	}

	ssize_t tupleSize = PyTuple_Size(obj);
	for (int i = 0; i < tupleSize; i++) {
		PyObject* tblObj = PyTuple_GetItem(obj, i);
		Table tbl;
		PyTableToTable(tblObj, &tbl);
		if (!dbHandler->replaceRow(&tbl))
		{
			Py_RETURN_FALSE;
		}
	}

	Py_RETURN_TRUE;
}

static PyObject* deleteRow(PyObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj)) {
		LOG_ERROR("delete row failed, args is error!!");
		Py_RETURN_FALSE;
	}
	DBHandler* dbHandler = ((PyDbObject*)self)->db_inst;
	if (dbHandler == NULL)
	{
		LOG_ERROR("delete row failed, db hander is null!!");
		Py_RETURN_FALSE;
	}

	Table tbl;
	PyTableToTable(obj, &tbl);

	if (!dbHandler->deleteRow(&tbl))
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


static PyObject* test(PyObject* self, PyObject* args)
{
	PyDbObject* obj = (PyDbObject*)self;
	printf("test exec------");
	Py_RETURN_NONE;
}

static PyMethodDef db_methods[] = {
	{"test", test, METH_NOARGS, "test"},
	{"executeSql", executeSql, METH_VARARGS, ""},
	{"initTable", initTable, METH_VARARGS, ""},
	{"insertRow", insertRow, METH_VARARGS, ""},
	{"getRow", getRow, METH_VARARGS, ""},
	{"updateRow", updateRow, METH_VARARGS, ""},
	{"replaceRows", replaceRows, METH_VARARGS, ""},
	{"deleteRow", deleteRow, METH_VARARGS, ""},
	{NULL, NULL}           /* sentinel */
};

static PyMemberDef db_members[] = {
	{"name", T_OBJECT_EX, offsetof(PyDbObject, name), 0, "first name"},
	{NULL}
};

TYPE_CONSTRUTOR(PyTypeDbInst)
{
}

TYPE_METHOD(PyTypeDbInst, db_methods);
TYPE_MEMBER(PyTypeDbInst, db_members);
TYPE_NEWFUNC(PyTypeDbInst, PyDbInst_New);
TYPE_FREEFUNC(PyTypeDbInst, PyDbInst_Free);
TYPE_REPRFUNC(PyTypeDbInst, db_repr);

