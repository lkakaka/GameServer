#include "DBHandler.h"

#include <iterator>
#include <exception>

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/datatype.h"
#include "jdbc/cppconn/prepared_statement.h"

#include "Logger.h"

using namespace sql;

USING_DATA_BASE;

typedef struct FieldMeta {
	std::string field;
	union FieldValue
	{
		int i;
		double b;
		//char s[64*1024];
		std::string s;
		FieldValue() {}
		~FieldValue() {}
	} val;
}FieldMeta;


DBHandler::~DBHandler()
{
	Logger::logInfo("$db hander destory!!!");
}

DBHandler::DBHandler(std::string& dbUrl, int dbPort, std::string& dbUserName, std::string& dbPassword, std::string dbName) :
	m_dbUrl(dbUrl), m_dbPort(dbPort), m_dbUserName(dbUserName), m_dbPassword(dbPassword), m_dbName(dbName), m_dbConn(NULL), 
	m_redis(new Redis("127.0.0.1", 6379))
{
	Connection* conn = getDBConnection();
	Statement* st = conn->createStatement();
	std::string sql = "CREATE DATABASE IF NOT EXISTS ";
	sql += dbName;
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	st->execute(sqlStr);
	initTableSchema();

	Table tbl;
	tbl.tableName = "player";
	tbl.priKeyVal = 13;
	TableField tblField;
	tblField.fieldName = "account";
	tblField.sval = "haha";
	tbl.addField(tblField);
	//insertRow(&tbl);
	getRow("player", 1);
}

void DBHandler::initTableSchema() {
	Connection* conn = getDBConnection();
	char* sql = "select TABLE_NAME from information_schema.tables where table_schema = '%s' and table_type = 'BASE TABLE'";
	StatementPtr ptr = executeSql(sql, m_dbName.c_str());
	Statement* st = ptr->getStatement();

	sql::ResultSet* rs = st->getResultSet();
	std::vector<std::string> vecTables;
	while (rs->next()) {
		vecTables.push_back(rs->getString(1).c_str());
	}

	for (auto iter = vecTables.begin(); iter != vecTables.end(); iter++) {
		std::shared_ptr<Table> ptrTable = std::make_shared<Table>();
		m_tableSchema.emplace(*iter, ptrTable);
		// 找主键
		sql = "select column_name from information_schema.key_column_usage where constraint_schema = '%s' and table_name = '%s' and constraint_name='PRIMARY'";
		ptr = executeSql(sql, m_dbName.c_str(), iter->c_str());
		st = ptr->getStatement();
		sql::ResultSet* rs = st->getResultSet();
		if (!rs->next()) {
			Logger::logError("$not primary key, table:%s", iter->c_str());
			exit(1);
		}

		std::string priKey = rs->getString(1).c_str();
		ptrTable->priKeyName = priKey;

		// 获取索引信息
		sql = "select * from(SELECT a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name, GROUP_CONCAT(column_name ORDER BY seq_in_index) AS `columns` "
			"FROM information_schema.statistics a "
			"GROUP BY a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name) as b where b.TABLE_NAME = '%s' and b.index_name LIKE 'Index_%'";
		ptr = executeSql(sql, iter->c_str());
		st = ptr->getStatement();
		rs = st->getResultSet();
		while (rs->next()) {
			std::string indexName = rs->getString(sql::SQLString("index_name"));
			std::string columns = rs->getString(sql::SQLString("columns"));
			TableIndex tblIndex;
			int npos = -1;
			while ((npos = columns.find_first_of(',', 0)) >= 0) {
				tblIndex.cols.push_back(columns.substr(0, npos));
				columns = columns.substr(npos + 1, columns.size() - npos);
			}
			if (!columns.empty()) tblIndex.cols.push_back(columns);
			ptrTable->tableIndexs.emplace_back(tblIndex);
		}


		// 获取字段信息
		sql = "select column_name, data_type, column_default from information_schema.columns where table_schema = '%s' and table_name = '%s'";
		ptr = executeSql(sql, m_dbName.c_str(), iter->c_str());
		st = ptr->getStatement();
		rs = st->getResultSet();
		while (rs->next()) {
			TableField field;
			field.fieldName = rs->getString(1).c_str();
			std::string type = rs->getString(2).c_str();
			std::string defaultVal = rs->getString(3).c_str();
			if (type == "int" || type == "tinyint") {
				field.type = TableField::FieldType::TYPE_INT;
			}
			else if (type == "bigint") {
				field.type = TableField::FieldType::TYPE_BIGINT;
			}
			else if (type == "double") {
				field.type = TableField::FieldType::TYPE_DOUBLE;
			}
			else {
				field.type = TableField::FieldType::TYPE_VCHAR;
			}
			field.defaut_val = defaultVal;
			ptrTable->fields.emplace(field.fieldName, field);
		}
	}
	//sql::ResultSetMetaData* metaData = rs->getMetaData();
}

TableField* DBHandler::getTableField(const char* tableName, const char* fieldName) const {
	auto iter = m_tableSchema.find(tableName);
	if (iter == m_tableSchema.end()) {
		Logger::logError("$not found table:%s", tableName);
		return NULL; 
	}
	auto fieldIter = iter->second->fields.find(fieldName);
	if (fieldIter == iter->second->fields.end()) {
		Logger::logError("$not found table's field, table:%s, field:%s", tableName, fieldName);
		return NULL;
	}

	return &fieldIter->second;
}

Table* DBHandler::getTableSchema(const char* tableName) const {
	auto iter = m_tableSchema.find(tableName);
	if (iter == m_tableSchema.end()) {
		Logger::logError("$not found table schema:%s", tableName);
		return NULL;
	}
	return iter->second.get();
}

inline std::string DBHandler::getDbName() { return m_dbName; }

Connection* DBHandler::getDBConnection()
{
	if (m_dbConn != NULL) {
		if (m_dbConn->isValid() || m_dbConn->reconnect()) {
			return m_dbConn;
		}
		m_dbConn->close();
	}
	Driver* driver = get_driver_instance();
	char urlBuf[32]{ 0 };
	sprintf(urlBuf, "tcp://%s:%d", m_dbUrl.c_str(), m_dbPort);
	std::string url = urlBuf; //"tcp://127.0.0.1:3306";
	if (!m_dbName.empty())
	{
		url += "/" + m_dbName;
	}
	sql::SQLString sqlUrl = sql::SQLString(url.c_str());
	sql::SQLString dbUserName = sql::SQLString(m_dbUserName.c_str());
	sql::SQLString dbPassword = sql::SQLString(m_dbPassword.c_str());
	m_dbConn = driver->connect(sqlUrl, dbUserName, dbPassword);
	return m_dbConn;
}



bool DBHandler::createTable(Table* tbl)
{
	std::string tbName = tbl->tableName;
	std::string colStr;
	int colNum = tbl->colNames.size();
	for (int col = 0; col < colNum; col++) {
		std::string colName = tbl->colNames[col];
		colStr += colName;
		TableField field = tbl->fields.find(colName)->second;
		switch (field.type)
		{
			case TableField::FieldType::TYPE_INT:
			{
				colStr += " INT";
				break;
			}
			case TableField::FieldType::TYPE_BIGINT:
			{
				colStr += " BIGINT";
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				colStr += " FLOAT";
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			{
				int length = field.length > 0 ? field.length : 128;
				char buf[64]{ 0 };
				snprintf(buf, 64, " VARCHAR(%ld)", length);
				colStr += buf;
				break;
			}
			case TableField::FieldType::TYPE_TEXT:
			{
				colStr += " TEXT";
				break;
			}
			default:
				Logger::logError("$not support table col type %ld, table:%s", field.type, tbName.c_str());
				return false;
		}

		if (field.defaut_val != "") {
			colStr += " DEFAULT " + field.defaut_val;
		}

		if (tbl->priKeyName == colName) {
			colStr += " primary key";
		}
		if (col != colNum - 1) colStr += ", ";
	}

	char* sql = "CREATE TABLE IF NOT EXISTS %s(%s)";
	StatementPtr ptr = executeSql(sql, tbName.c_str(), colStr.c_str());
	if (ptr == NULL) {
		Logger::logError("$creat table %s failed", tbName.c_str());
		return false;
	}

	sql = "select * from(SELECT a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name, GROUP_CONCAT(column_name ORDER BY seq_in_index) AS `columns` "
		"FROM information_schema.statistics a "
		"GROUP BY a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name) as b where b.TABLE_NAME = '%s' and b.index_name LIKE 'Index_%%'";
	ptr = executeSql(sql, tbName.c_str());
	Statement* st = ptr->getStatement();
	sql::ResultSet* rs = st->getResultSet();
	std::set<std::string> indexs;
	while (rs->next()) {
		std::string indexName = rs->getString("index_name").c_str();
		indexs.emplace(indexName);
	}

	// 初始化索引
	for (auto iter = tbl->tableIndexs.begin(); iter != tbl->tableIndexs.end(); iter++) {
		std::string indexName = "Index_";
		std::string colVals;
		for (std::string s : iter->cols) {
			indexName += s + "_";
			colVals += s + ",";
		}
		// 去掉最后的逗号
		if (!colVals.empty()) {
			colVals.pop_back();
			indexName.pop_back();
		}

		// 索引已经创建
		if (indexs.erase(indexName) > 0) {
			continue;
		}

		char* sql = "CREATE %s INDEX %s ON %s(%s)";
		StatementPtr ptr = executeSql(sql, iter->isUnique ? "UNIQUE" : "", indexName.c_str(), tbName.c_str(), colVals.c_str());
		if (ptr == NULL) {
			Logger::logError("$creat index failed, table:%s, cols:%s", tbName.c_str(), colVals.c_str());
			return false;
		}
	}

	// 删除不用的索引
	for (std::string indexName : indexs) {
		char* sql = "DROP INDEX %s ON %s";
		StatementPtr ptr = executeSql(sql, indexName.c_str(), tbName.c_str());
		if (ptr == NULL) {
			Logger::logError("$drop index failed, table:%s, index:%s", tbName.c_str(), indexName.c_str());
			return false;
		}
	}

	return true;
}



void DBHandler::initDbTable(std::vector<ReflectObject*> tblDefs)
{
	/*for (auto iter = tblDefs.begin(); iter != tblDefs.end(); iter++) {
		createTable(*iter);
	}*/
}

//void DBHandler::createTable(ReflectObject* tbl)
//{
//	std::string sql = "CREATE TABLE IF NOT EXISTS " + tbl->getTypeName() + "(";
//	std::map<std::string, Field> fieldMap = tbl->getFieldMap();
//	if (fieldMap.size() == 0) {
//		Logger::logError("$create table %s failed, not define any field ", tbl->getTypeName().c_str());
//		return;
//	}
//	for (auto iter = fieldMap.begin(); iter != fieldMap.end(); iter++) {
//		Field field = iter->second;
//		std::string filedStr = field.name;
//		switch (field.type)
//		{
//		case TYPE_INT:
//			filedStr += " INT,";
//			break;
//		case TYPE_STRING:
//			filedStr += " VARCHAR(128),";
//			break;
//		default:
//			Logger::logError("$create table %s failed, unsupport db field type: %s", tbl->getTypeName().c_str(), field.type);
//			return;
//		}
//		sql += filedStr;
//	}
//	sql.pop_back();
//	sql += ")";
//	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
//	Connection* conn = getDBConnection();
//	if (conn == NULL) {
//		Logger::logError("$exec sql failed, conn is null, sql: %s", sql.c_str());
//		return;
//	}
//	Statement* st = conn->createStatement();
//	try {
//		st->execute(sqlStr);
//	}
//	catch (std::exception e) {
//		Logger::logError("$exec sql failed, sql: %s, e: %s", sql.c_str(), e.what());
//	}
//}
//
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

StatementPtr DBHandler::executeSql(const char* sqlFormat, ...)
{
	va_list va;
	va_start(va, sqlFormat);
	int n = vsnprintf(m_sqlBuf, MAX_SQL_LENGTH, sqlFormat, va);
	if (n >= MAX_SQL_LENGTH) {
		Logger::logError("$sql length exceed %d, sql: %s", MAX_SQL_LENGTH, m_sqlBuf);
	}
	va_end(va);
	Connection* conn = getDBConnection();
	if (conn == NULL) {
		Logger::logError("$exec sql failed, conn is null, sql: %s", m_sqlBuf);
		return NULL;
	}
	Statement* st = conn->createStatement();
	sql::SQLString sqlStr = sql::SQLString(m_sqlBuf);
	try {
		bool isResultSet = st->execute(sqlStr);
		Logger::logInfo("$exec sql success, sql: %s", m_sqlBuf);
		return MAKE_STATEMENT_PTR(st, isResultSet);
	}
	catch (std::exception& e) {
		Logger::logError("$execute sql failed, sql:%s, ex: %s", m_sqlBuf, e.what());
	}
	return NULL;
}

void DBHandler::addUpdateRecord(std::string& tableName, long keyVal, std::set<std::string> stFields)
{
	auto iter = m_chgRecords.find(tableName);
	if (iter == m_chgRecords.end()) {
		std::map<long, std::set<std::string>> record;
		m_chgRecords.emplace(tableName, record);
		iter = m_chgRecords.find(tableName);
	}

	std::map<long, std::set<std::string>> mpRecord = iter->second;
	auto mpIter = mpRecord.find(keyVal);
	if (mpIter == mpRecord.end()) {
		std::set<std::string> st;
		mpRecord.emplace(keyVal, st);
		mpIter = mpRecord.find(keyVal);
	}

	std::set<std::string> st = mpIter->second;
	for (auto iter1 = stFields.begin(); iter1 != stFields.end(); iter1++) {
		if (st.find(*iter1) != st.end()) continue;
		st.insert(*iter1);
	}
}

std::shared_ptr<Table> DBHandler::getRowFromRedis(std::string& tableName, long keyVal)
{
	std::string redisKey = Table::redisKey(tableName.c_str(), keyVal);
	REDIS_REPLY_PTR reply_ptr = m_redis->execRedisCmd("hgetall %s", redisKey);
	if (reply_ptr == NULL) return NULL;
	std::shared_ptr<Table> tbl = std::make_shared<Table>();
	redisReply* reply = reply_ptr->getReply();
	for (int i = 0; i < reply->elements / 2; i+=2) {
		redisReply* keyReply = reply->element[i];
		TableField field;
		if (keyReply->len > 0) {
			std::copy(keyReply->str, keyReply->str + keyReply->len, std::back_inserter(field.fieldName));
		}
		redisReply* valReply = reply->element[i+1];
		if (valReply->len > 0) {
			std::copy(valReply->str, valReply->str + valReply->len, std::back_inserter(field.sval));
		}
		
		tbl->addField(field);
	}
	return tbl;
}

void DBHandler::flushChgRedisRecordToDB()
{
	Logger::logInfo("$start flush redis record to db!!");
	for (auto iter = m_chgRecords.begin(); iter != m_chgRecords.end(); iter++) {
		std::string tableName = iter->first;
		std::map<long, std::set<std::string>> mpRecord = iter->second;
		Table* tblSchema = getTableSchema(tableName.c_str());
		if (tblSchema == NULL) continue;
		for (auto iter1 = mpRecord.begin(); iter1 != mpRecord.end(); iter1++) {
			std::string sql = "update " + tableName + " ";
			long keyVal = iter1->first;
			std::set<std::string> st = iter1->second;
			std::string redisKey = Table::redisKey(tableName.c_str(), iter1->first);
			std::shared_ptr<Table> tbl = getRowFromRedis(tableName, keyVal);
			
			bool isFirst = true;
			for (auto iter2 = st.begin(); iter2 != st.end(); iter2++) {
				auto iter3 = tbl->fields.find(*iter2);
				if (iter3 == tbl->fields.end()) continue;
				if (!isFirst) {
					sql += ",";
					isFirst = true;
				}
				sql += *iter2 + "=" + iter3->second.sval;
			}
			sql += " where " + tblSchema->priKeyName + "=%ld";
			executeSql(sql.c_str(), keyVal);
		}
	}
	m_chgRecords.clear();
	Logger::logInfo("$flush redis record to db finished!!");
}

bool DBHandler::checkRedisExistAndLoad(const char* tableName, long keyVal) {
	std::string redisKey;
	char buf[64];
	snprintf(buf, 64, "%ld", keyVal);
	redisKey.append(tableName).append(":").append(buf);
	std::shared_ptr<RedisReply> reply = m_redis->execRedisCmd("EXISTS %s", redisKey.c_str());
	if (reply == NULL) return false;
	// redis中已经存在
	if (reply->getReply()->integer == 1) return true;
	// redis中不存在从数据库加载
	Table* tbl = getTableSchema(tableName);
	if (tbl == NULL) {
		return false;
	}
	std::string formatSql = "SELECT * FROM %s WHERE %s=%s";
	StatementPtr ptr = executeSql(formatSql.c_str(), tableName, tbl->priKeyName, keyVal);
	if (ptr == NULL) return false;
	Statement* st = ptr->getStatement();
	sql::ResultSet* rs = st->getResultSet();
	sql::ResultSetMetaData* metaData = rs->getMetaData();
	int colCount = metaData->getColumnCount();
	bool hasNext = rs->next();
	std::string redisCmd = "hmset " + redisKey;
	for (int i = 1; i <= colCount; i++) {
		redisCmd += " " + metaData->getColumnLabel(i) + " ";
		int colType = metaData->getColumnType(i);
		if (colType >= sql::DataType::BIT && colType <= sql::DataType::BIGINT) {
			int val = hasNext ? rs->getInt(i) : 0;
			redisCmd += val;
		}
		else if (colType >= sql::DataType::REAL && colType <= sql::DataType::NUMERIC) {
			double val = hasNext ? rs->getDouble(i) : 0;
			redisCmd += val;
		}
		else {
			const char* val = hasNext ? rs->getString(i).c_str() : "";
			redisCmd += val;
		}
	}
	m_redis->execRedisCmd(redisCmd.c_str());
	return true;
}

bool DBHandler::insertRow(Table* tbl)
{
	std::string fields;
	std::string vals;
	Table* tblSchema = getTableSchema(tbl->tableName.c_str());
	bool isSetPriKey = false;
	for (auto iter = tbl->fields.begin(); iter != tbl->fields.end(); iter++) {
		std::string fieldName = iter->first;
		TableField* tbField = getTableField(tbl->tableName.c_str(), fieldName.c_str());
		if (tbField == nullptr) {
			return false;
		}

		bool isPriKey = false;
		if (!isSetPriKey && fieldName == tblSchema->priKeyName) {
			isSetPriKey = true;
			isPriKey = true;
		}
		TableField field = iter->second;
		fields += fieldName + ",";
		switch (tbField->type) {
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field.lval);
				vals += buf;
				if (isPriKey) tbl->priKeyVal = field.lval;
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field.dval);
				vals += buf;
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				vals += "'" + field.sval + "'";
				break;
			}
			default:
				Logger::logError("$not support field type, table:%s, field:%s, type:%d", tbl->tableName.c_str(), fieldName.c_str(), tbField->type);
				return false;
		}
		vals += ",";
	}

	// 删除最后的逗号
	if (!fields.empty()) {
		fields.pop_back();
		vals.pop_back();
	}

	char* sql = "INSERT INTO %s(%s) VALUES(%s)";
	StatementPtr ptr = executeSql(sql, tbl->tableName.c_str(), fields.c_str(), vals.c_str());
	if (ptr == NULL) return false;
	int updateCount = ptr->getStatement()->getUpdateCount();
	if (updateCount <= 0) {
		Logger::logError("$exe insert sql failed, sql:%s, fields:%s, vals:%s", sql, fields.c_str(), vals.c_str());
		return false;
	}

	if (!isSetPriKey) {
		ptr = executeSql("SELECT last_insert_id()");
		Statement* st = ptr->getStatement();
		ResultSet* rs = st->getResultSet();
		if (rs->next()) {
			tbl->priKeyVal = rs->getInt64(1);
		} else {
			Logger::logError("$exe insert sql error, not found laster insert id, sql:%s, fields:%s, vals:%s", sql, fields.c_str(), vals.c_str());
		}
	}

	return true;
}

REDIS_REPLY_PTR DBHandler::getRow(const char* tableName, long keyVal)
{
	if (!checkRedisExistAndLoad(tableName, keyVal)) return NULL;

	std::string redisKey = Table::redisKey(tableName, keyVal);
	return m_redis->execRedisCmd("HGETALL %s", redisKey.c_str());
}

bool DBHandler::updateRow(Table* tbl)
{
	if (!checkRedisExistAndLoad(tbl->tableName.c_str(), tbl->priKeyVal)) return false;
	std::string redisCmd = "hmset " + tbl->redisKey();
	std::set<std::string> stFields;
	for (auto iter = tbl->fields.begin(); iter != tbl->fields.end(); iter++) {
		std::string fieldName = iter->first;
		stFields.insert(fieldName);
		TableField field = iter->second;
		redisCmd.append(" " + fieldName + " ");
		switch (field.type) {
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field.lval);
				redisCmd.append(buf);
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field.dval);
				redisCmd.append(buf);
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				redisCmd.append("'" + field.sval + "'");
				break;
			}
			default:
				Logger::logError("$not support field type, table:%s, field:%s, type:%d", tbl->tableName.c_str(), fieldName.c_str(), field.type);
				return false;
		}
	}
	m_redis->execRedisCmd(redisCmd.c_str());
	addUpdateRecord(tbl->tableName, tbl->priKeyVal, stFields);
	flushChgRedisRecordToDB();
	return true;
}

bool DBHandler::deleteRow(Table* tbl)
{
	std::string redisCmd = "del " + tbl->redisKey();
	REDIS_REPLY_PTR replyPtr = m_redis->execRedisCmd(redisCmd.c_str());
	if (replyPtr == NULL) return false;
	executeSql("delete from %s where %s=%s", tbl->tableName, tbl->priKeyName, tbl->priKeyVal);
	return true;
}
