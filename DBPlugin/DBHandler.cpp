#include "DBHandler.h"

#include <iterator>
#include <exception>

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/datatype.h"

#include "Logger.h"

using namespace sql;

//Connection* getDBConnection(std::string dbName)
//{
//	Driver* driver = get_driver_instance();
//	std::string url = "tcp://127.0.0.1:3306";
//	if (!dbName.empty())
//	{
//		url += "/" + dbName;
//	}
//	sql::SQLString sqlUrl = sql::SQLString(url.c_str());
//	Connection* m_dbConn = driver->connect(sqlUrl, "root", "");
//	return m_dbConn;
//}

DBHandler::~DBHandler()
{
	Logger::logInfo("$db hander destory!!!");
}

Connection* DBHandler::getDBConnection()
{
	if (m_dbConn.get() != NULL) {
		return m_dbConn.get();
	}
	Driver* driver = get_driver_instance();
	char urlBuf[32]{0};
	sprintf(urlBuf, "tcp://%s:%d", m_dbUrl.c_str(), m_dbPort);
	std::string url = urlBuf; //"tcp://127.0.0.1:3306";
	if (!m_dbName.empty())
	{
		url += "/" + m_dbName;
	}
	sql::SQLString sqlUrl = sql::SQLString(url.c_str());
	sql::SQLString dbUserName = sql::SQLString(m_dbUserName.c_str());
	sql::SQLString dbPassword = sql::SQLString(m_dbPassword.c_str());
	Connection* dbConn = driver->connect(sqlUrl, dbUserName, dbPassword);
	m_dbConn.reset(dbConn);
	return dbConn;
}

DBHandler::DBHandler(std::string& dbUrl, int dbPort, std::string& dbUserName, std::string& dbPassword, std::string dbName) :
	m_dbUrl(dbUrl), m_dbPort(dbPort), m_dbUserName(dbUserName), m_dbPassword(dbPassword), m_dbName(dbName)
{
	Connection* conn = getDBConnection();
	if (conn != NULL) {
		Statement* st = conn->createStatement();
		std::string sql = "CREATE DATABASE IF NOT EXISTS ";
		sql += dbName;
		sql::SQLString sqlStr = sql::SQLString(sql.c_str());
		st->execute(sqlStr);
	}
}

inline std::string DBHandler::getDbName() { return m_dbName; }

void DBHandler::initDbTable(std::vector<ReflectObject*> tblDefs)
{
	for (auto iter = tblDefs.begin(); iter != tblDefs.end(); iter++) {
		createTable(*iter);
	}
}

void DBHandler::createTable(ReflectObject* tbl)
{
	std::string sql = "CREATE TABLE IF NOT EXISTS " + tbl->getTypeName() + "(";
	std::map<std::string, Field> fieldMap = tbl->getFieldMap();
	if (fieldMap.size() == 0) {
		Logger::logError("$create table %s failed, not define any field ", tbl->getTypeName().c_str());
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
			Logger::logError("$create table %s failed, unsupport db field type: %s", tbl->getTypeName().c_str(), field.type);
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
		return;
	}
	Statement* st = conn->createStatement();
	try {
		st->execute(sqlStr);
	}
	catch (std::exception e) {
		Logger::logError("$exec sql failed, sql: %s, e: %s", sql.c_str(), e.what());
	}
}

void DBHandler::insert(std::vector<ReflectObject> data)
{
	for (auto iter = data.begin(); iter != data.end(); iter++) {
		insertOne(*iter);
	}
}

void DBHandler::insertOne(ReflectObject tbl)
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
			Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName().c_str(), field.type);
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
		return;
	}
	Statement* st = conn->createStatement();
	st->execute(sqlStr);
}

void DBHandler::select(ReflectObject tbl)
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
			Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName().c_str(), field.type);
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
		return;
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
				Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName().c_str(), field1.type);
				return;
			}
		}
	}
}

void DBHandler::update(ReflectObject src, ReflectObject dst)
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
				Logger::logError("update table %s failed, unsupport db field type: %s", src.getTypeName().c_str(), field.type);
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
				Logger::logError("update table %s failed, unsupport db field type: %s", dst.getTypeName().c_str(), field.type);
				return;
			}
		}
	}

	sql += " SET " + val + " WHERE " + conditions;
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
		return;
	}
	Statement* st = conn->createStatement();
	st->execute(sqlStr);
}

void DBHandler::del(ReflectObject tbl)
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
			Logger::logError("insert table %s failed, unsupport db field type: %s", tbl.getTypeName().c_str(), field.type);
			return;
		}
	}

	if (conditions == "") {
		Logger::logError("del table %s failed, not assign condition", tbl.getTypeName().c_str());
		return;
	}

	sql += " WHERE " + conditions;
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
		return;
	}
	Statement* st = conn->createStatement();
	st->execute(sqlStr);
}

void DBHandler::executeSql(std::string sql, std::function<void(Statement*, bool)> handler)
{
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
		return;
	}
	Statement* st = conn->createStatement();
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	bool isResultSet = st->execute(sqlStr);
	if (handler != NULL) {
		handler(st, isResultSet);
	}
	Logger::logInfo("$exec sql success, sql: %s", sql.c_str());
	st->close();

	//while (true) {
	//	if (isResultSet) {
	//		ResultSet* rs = st->getResultSet();
	//		ResultSetMetaData* metaData = rs->getMetaData();
	//		std::map<std::string, int> fieldTypeMap;
	//		for (int i = 1; i <= metaData->getColumnCount(); i++) {
	//			std::string fieldName = metaData->getColumnName(i);
	//			int colType = metaData->getColumnType(i);
	//			if (colType == DataType::INTEGER) {
	//			}
	//		}
	//		while (rs->next()) {
	//			
	//		}
	//	}
	//	else {
	//		int updateCount = st->getUpdateCount();
	//		if (updateCount < 0) {
	//			break;
	//		}
	//		//Logger::logError("$exec sql success, sql: %s, update count:%d", sql.c_str(), updateCount);
	//	}
	//	
	//	isResultSet = st->getMoreResults();
	//}
	
}
