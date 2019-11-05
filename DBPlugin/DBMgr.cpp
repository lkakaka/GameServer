#include "DBMgr.h"

#include <iterator>
#include <exception>

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"

#include "Logger.h"

using namespace sql;

DBMgr::DBMgr(std::string dbName)
{
	m_dbName = dbName;
}

static Connection* getDBConnection()
{
	Driver* driver = get_driver_instance();
	Connection* m_dbConn = driver->connect("tcp://127.0.0.1:3306/test1", "root", "");
	return m_dbConn;
}

void DBMgr::initDbTable(std::vector<ReflectObject*> tblDefs)
{
	Logger::initLog();
	for (auto iter = tblDefs.begin(); iter != tblDefs.end(); iter++) {
		createTable(*iter);
	}
}

void DBMgr::createTable(ReflectObject* tbl)
{	
	std::string sql = "CREATE TABLE IF NOT EXISTS " + tbl->getTypeName() + "(";
	std::map<std::string, Field> fieldMap = tbl->getFieldMap();
	if (fieldMap.size() == 0) {
		Logger::logError("$create table %s failed, not define any field ", tbl->getTypeName());
		return;
	}
	for (auto iter = fieldMap.begin(); iter != fieldMap.end(); iter++) {
		Field field = iter->second;
		std::string filedStr = field.name;
		switch (field.type)
		{
		case TYPE_INT:
			filedStr += " INT,";
			break;
		case TYPE_STRING:
			filedStr += " VARCHAR(128),";
			break;
		default:
			Logger::logError("$create table %s failed, unsupport db field type: %s", tbl->getTypeName(), field.type);
			return;
		}
		sql += filedStr;
	}
	sql.pop_back();
	sql += ")";
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
	}
	Statement* st = conn->createStatement();
	try {
		st->execute(sqlStr);
	}
	catch (std::exception e) {
		Logger::logError("$exec sql failed, sql: %s, e: %s", sql.c_str(), e.what());
	}
}

void DBMgr::insert(std::vector<ReflectObject> data)
{
	for (auto iter = data.begin(); iter != data.end(); iter++) {
		insertOne(*iter);
	}
}

void DBMgr::insertOne(ReflectObject tbl)
{
	std::string sql = "INSERT INTO " + tbl.getTypeName() + " (";
	std::string val = " VALUES(";
	std::map<std::string, Field> fieldMap = tbl.getFieldMap();
	for (auto iter = fieldMap.begin(); iter != fieldMap.end(); iter++) {
		Field field = iter->second;
		sql += field.name + ",";
		char buff[32]{ 0 };
		switch (field.type)
		{
		case TYPE_INT:
			sprintf(buff, "%d,", tbl.getInt(field.name));
			val += buff;
			break;
		case TYPE_STRING:
			val = val + "\"" + tbl.getString(field.name) + "\",";
			break;
		default:
			Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName(), field.type);
			return;
		}
	}
	sql.pop_back();
	val.pop_back();

	sql += ")" + val + ")";
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
	}
	Statement* st = conn->createStatement();
	st->execute(sqlStr);
}

void DBMgr::select(ReflectObject tbl)
{
	std::string sql = "SELECT * FROM " + tbl.getTypeName();
	std::map<std::string, Field> fieldMap = tbl.getFieldMap();
	std::string conditions = "";
	for (auto iter = fieldMap.begin(); iter != fieldMap.end(); iter++) {
		Field field = iter->second;
		if (!field.isSet) {
			continue;
		}
		if (conditions != "") {
			conditions += " AND ";
		}
		conditions += field.name + "=";
		char buff[32]{ 0 };
		switch (field.type)
		{
		case TYPE_INT:
			sprintf(buff, "%d", tbl.getInt(field.name));
			conditions += buff;
			break;
		case TYPE_STRING:
			conditions += "\"" + tbl.getString(field.name) + "\"";
			break;
		default:
			Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName(), field.type);
			return;
		}
	}

	if (conditions != "") {
		sql += " WHERE " + conditions;
	}

	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
	}
	Statement* st = conn->createStatement();
	ResultSet* result = st->executeQuery(sqlStr);

	/*ResultSetMetaData* metaData = result->getMetaData();
	for (int i = 1; i <= metaData->getColumnCount(); i++) {
		std::string fieldName = metaData->getColumnName(i);
		result->getRow
	}*/

	while (result->next()) {
		for (auto iter = fieldMap.begin(); iter != fieldMap.end(); iter++) {
			Field field1 = iter->second;
			int val;
			SQLString val1;
			switch (field1.type)
			{
				case TYPE_INT:
					val = result->getInt(field1.name);
					break;
				case TYPE_STRING:
					val1 = result->getString(field1.name);
					break;
				default:
					Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName(), field1.type);
					return;
			}
		}
	}
}

void DBMgr::update(ReflectObject src, ReflectObject dst)
{
	std::string sql = "UPDATE " + src.getTypeName();
	std::map<std::string, Field> fieldMap = src.getFieldMap();
	std::string conditions = "";
	std::string val = "";
	for (auto iter = fieldMap.begin(); iter != fieldMap.end(); iter++) {
		Field field = iter->second;
		if (field.isSet) {
			if (conditions != "") {
				conditions += " AND ";
			}
			conditions += field.name + "=";
			char buff[32]{ 0 };
			switch (field.type)
			{
			case TYPE_INT:
				sprintf(buff, "%d", src.getInt(field.name));
				conditions += buff;
				break;
			case TYPE_STRING:
				conditions += "\"" + src.getString(field.name) + "\"";
				break;
			default:
				Logger::logError("update table %s failed, unsupport db field type: %s", src.getTypeName(), field.type);
				return;
			}
		}

		Field* dstField = dst.getField(field.name);
		if (dstField != NULL && dstField->isSet) {
			if (val != "") {
				val += ",";
			}
			val += field.name + "=";
			char buff[32]{ 0 };
			switch (field.type)
			{
			case TYPE_INT:
				sprintf(buff, "%d", dst.getInt(field.name));
				conditions += buff;
				break;
			case TYPE_STRING:
				conditions += "\"" + dst.getString(field.name) + "\"";
				break;
			default:
				Logger::logError("update table %s failed, unsupport db field type: %s", dst.getTypeName(), field.type);
				return;
			}
		}
	}

	sql += " SET " + val + " WHERE " + conditions;
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
	}
	Statement* st = conn->createStatement();
	st->execute(sqlStr);
}

void DBMgr::del(ReflectObject tbl)
{
	std::string sql = "DELETE FROM " + tbl.getTypeName();
	std::map<std::string, Field> fieldMap = tbl.getFieldMap();
	std::string conditions = "";
	for (auto iter = fieldMap.begin(); iter != fieldMap.end(); iter++) {
		Field field = iter->second;
		if (!field.isSet) {
			continue;
		}
		if (conditions != "") {
			conditions += " AND ";
		}
		conditions += field.name + "=";
		char buff[32]{ 0 };
		switch (field.type)
		{
		case TYPE_INT:
			sprintf(buff, "%d", tbl.getInt(field.name));
			conditions += buff;
			break;
		case TYPE_STRING:
			conditions += "\"" + tbl.getString(field.name) + "\"";
			break;
		default:
			Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName(), field.type);
			return;
		}
	}

	if (conditions == "") {
		Logger::logError("del table %s failed, not assign condition", tbl.getTypeName());
		return;
	}

	sql += " WHERE " + conditions;
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
	}
	Statement* st = conn->createStatement();
	st->execute(sqlStr);
}

void DBMgr::executeSql(std::string sql)
{
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
	}
	Statement* st = conn->createStatement();
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	st->execute(sqlStr);
}