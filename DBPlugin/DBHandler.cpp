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

	//Table tbl;
	//tbl.tableName = "player";
	//tbl.priKeyVal = 13;
	//TableField tblField;
	//tblField.fieldName = "account";
	//tblField.sval = "haha";
	//tbl.addField(tblField);
	////insertRow(&tbl);
	//getRow("player", 1);
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
		std::shared_ptr<TableSchema> ptrTable = std::make_shared<TableSchema>();
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
			"GROUP BY a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name) as b where b.TABLE_NAME = '%s' and b.index_name LIKE 'Index_%%'";
		ptr = executeSql(sql, iter->c_str());
		st = ptr->getStatement();
		rs = st->getResultSet();
		while (rs->next()) {
			TableIndex tblIndex;
			tblIndex.indexName = rs->getString(sql::SQLString("index_name")).c_str();
			std::string columns = rs->getString(sql::SQLString("columns")).c_str();
			int npos = -1;
			while ((npos = columns.find_first_of(',', 0)) >= 0) {
				tblIndex.cols.push_back(columns.substr(0, npos));
				columns = columns.substr(npos + 1, columns.size() - npos);
			}
			if (!columns.empty()) tblIndex.cols.push_back(columns);
			ptrTable->tableIndexs.emplace_back(tblIndex);
		}


		// 获取字段信息
		std::vector<TableField> fields;
		getTableCols(iter->c_str(), fields);
		for (TableField field : fields) {
			ptrTable->addFieldSchema(field);
		}
	}
}

void DBHandler::getTableCols(const char* tableName, std::vector<TableField>& fields) {
	char* sql = "select column_name, data_type, column_default, character_maximum_length from information_schema.columns where table_schema = '%s' and table_name = '%s'";
	StatementPtr ptr = executeSql(sql, m_dbName.c_str(), tableName);
	Statement* st = ptr->getStatement();
	ResultSet* rs = st->getResultSet();
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
		else if (type == "varchar") {
			field.type = TableField::FieldType::TYPE_VCHAR;
			field.length = rs->getInt(4);
		}
		else if (type == "text") {
			field.type = TableField::FieldType::TYPE_TEXT;
		}
		else {
			Logger::logError("$unkown col type:%s, table:%s", type, tableName);
		}
		field.defaut_val = defaultVal;
		fields.emplace_back(field);
	}
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

TableSchema* DBHandler::getTableSchema(const char* tableName) const {
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


bool isSameCol(TableField* col1, TableField* col2) {
	if (col1->fieldName != col2->fieldName) return false;
	if (col1->type != col2->type) return false;
	if (col1->length != col2->length) return false;
	if (col1->defaut_val != col2->defaut_val) return false;
	return true;
}

bool DBHandler::createTable(Table* tbl)
{
	std::string tbName = tbl->tableName;

	char* sql = "select TABLE_NAME from information_schema.tables where table_schema='%s' and table_name='%s'";
	StatementPtr ptr = executeSql(sql, m_dbName.c_str(), tbName.c_str());
	Statement* st = ptr->getStatement();
	ResultSet* rs = st->getResultSet();
	if (rs->next()) { // 表存在
		std::vector<TableField> fields;
		getTableCols(tbName.c_str(), fields);
		std::map<std::string, TableField> mpFields;
		for (TableField field : fields) {
			mpFields.emplace(field.fieldName, field);
		}
		_changeTable(tbl, mpFields);
	}
	else {
		_createNewTable(tbl);
	}

	// 查询索引
	sql = "select * from(SELECT a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name, GROUP_CONCAT(column_name ORDER BY seq_in_index) AS `columns` "
		"FROM information_schema.statistics a "
		"GROUP BY a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name) as b where b.TABLE_NAME = '%s' and b.index_name LIKE 'Index_%%'";
	ptr = executeSql(sql, tbName.c_str());
	st = ptr->getStatement();
	rs = st->getResultSet();
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

bool getColTypeStr(TableField* field, char* str, int len) {
	switch (field->type)
	{
		case TableField::FieldType::TYPE_INT: 
			snprintf(str, len, "INT");
			return true;
		case TableField::FieldType::TYPE_BIGINT: 
			snprintf(str, len, "BIGINT");
			return true;
		case TableField::FieldType::TYPE_DOUBLE:
			snprintf(str, len, "FLOAT");
			return true;
		case TableField::FieldType::TYPE_VCHAR:
		{
			int length = field->length > 0 ? field->length : 128;
			snprintf(str, len, " VARCHAR(%ld)", length);
			return true;
		}
		case TableField::FieldType::TYPE_TEXT:
			snprintf(str, len, "TEXT");
			return true;
		default:
		{
			Logger::logError("$not support table col type %d", field->type);
			return false;
		}
	}
}

bool DBHandler::_createNewTable(Table* tbl) {
	std::string tbName = tbl->tableName;
	std::string colStr;
	int colNum = tbl->colNames.size();
	for (int col = 0; col < colNum; col++) {
		std::string colName = tbl->colNames[col];
		TableField* field = tbl->getField(colName.c_str());
		colStr += colName + " ";
		char typeStr[64]{ 0 };
		if (!getColTypeStr(field, typeStr, 64)) return false;
		colStr += typeStr;

		if (field->defaut_val != "") {
			colStr += " DEFAULT " + field->defaut_val;
		}

		if (tbl->priKeyName == colName) {
			colStr += " primary key";
		}
		if (col != colNum - 1) colStr += ", ";
	}

	char* sql = "CREATE TABLE %s(%s)";
	StatementPtr ptr = executeSql(sql, tbName.c_str(), colStr.c_str());
	if (ptr == NULL) {
		Logger::logError("$creat table %s failed", tbName.c_str());
		return false;
	}

	return true;
}

bool DBHandler::_changeTable(Table* tbl, std::map<std::string, TableField>& orgFields) {
	std::string tbName = tbl->tableName;
	int colNum = tbl->colNames.size();
	bool isChange = false;
	std::string colStr;
	for (int col = 0; col < colNum; col++) {
		std::string colName = tbl->colNames[col];
		TableField* field = tbl->getField(colName.c_str()); //tbl->fields.find(colName)->second;
		
		if (field->isDel) { // 删除字段
			if (orgFields.find(field->fieldName) == orgFields.end()) {
				Logger::logWarning("$delete column %s in table %s not found", field->fieldName.c_str(), tbName.c_str());
				continue;
			}
			colStr += "DROP " + colName;
		} else { // 修改字段
			if (field->oldName == "") {
				auto iter = orgFields.find(colName);
				if (iter == orgFields.end()) { // 新加字段
					colStr += "ADD ";
				}
				else {  // 修改字段
					if (isSameCol(field, &iter->second)) {
						continue;
					}
					colStr += "MODIFY ";
				}
			}
			else { // 字段改名
				auto iter = orgFields.find(field->oldName);
				if (iter == orgFields.end()) { // 未找到原字段
					if (orgFields.find(field->fieldName) == orgFields.end()) {
						Logger::logError("$change column error, not found %s in table %s", field->oldName.c_str(), tbName.c_str());
						return false;
					}
					continue;
				}
				colStr += "CHANGE " + field->oldName + " ";
			}

			colStr += colName + " ";

			char typeStr[64]{ 0 };
			if (!getColTypeStr(field, typeStr, 64)) return false;
			colStr += typeStr;

			if (field->defaut_val != "") {
				colStr += " DEFAULT " + field->defaut_val;
			}

			if (tbl->priKeyName == colName) {
				colStr += " primary key";
			}
		}

		if (col != colNum - 1) colStr += ", ";
		if (!isChange) isChange = true;
	}

	if (!isChange) return true;

	char* sql = "ALTER TABLE %s %s";
	StatementPtr ptr = executeSql(sql, tbName.c_str(), colStr.c_str());
	if (ptr == NULL) {
		Logger::logError("$alter table %s failed", tbName.c_str());
		return false;
	}
	Logger::logInfo("$alter table %s, %s", tbName.c_str(), colStr.c_str());
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
		MAKE_TABLE_FIELD(field);
		if (keyReply->len > 0) {
			std::copy(keyReply->str, keyReply->str + keyReply->len, std::back_inserter(field->fieldName));
		}
		redisReply* valReply = reply->element[i+1];
		if (valReply->len > 0) {
			std::copy(valReply->str, valReply->str + valReply->len, std::back_inserter(field->sval));
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
		TableSchema* tblSchema = getTableSchema(tableName.c_str());
		if (tblSchema == NULL) continue;
		for (auto iter1 = mpRecord.begin(); iter1 != mpRecord.end(); iter1++) {
			std::string sql = "update " + tableName + " ";
			long keyVal = iter1->first;
			std::set<std::string> st = iter1->second;
			std::string redisKey = Table::redisKey(tableName.c_str(), iter1->first);
			std::shared_ptr<Table> tbl = getRowFromRedis(tableName, keyVal);
			
			bool isFirst = true;
			for (auto iter2 = st.begin(); iter2 != st.end(); iter2++) {
				TableField* tbField = tbl->getField(iter2->c_str());
				if (tbField == NULL) continue;
				if (!isFirst) {
					sql += ",";
					isFirst = true;
				}
				sql += *iter2 + "=" + tbField->sval;
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
	TableSchema* tbl = getTableSchema(tableName);
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

void DBHandler::loadFromDB(Table* tbl, std::vector<Table>& result) {
	std::string tbName = tbl->tableName;
	std::string colStr;
	TableSchema* tblSchema = getTableSchema(tbl->tableName.c_str());
	int size = tbl->fields.size();
	int count = 0;
	for (auto iter = tbl->fields.begin(); iter != tbl->fields.end(); iter++) {
		count++;
		TableField* field = iter->second.get();
		colStr += field->fieldName + "=";
		TableField* tbField = getTableField(tbName.c_str(), field->fieldName.c_str());
		switch (tbField->type)
		{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field->lval);
				colStr += buf;
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field->lval);
				colStr += buf;
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				colStr += "'" + field->sval + "'";
				break;
			}
			default:
			{
				Logger::logError("$not support field type:%d, table:%s, col:%s", tbField->type, tbName.c_str(), field->fieldName.c_str());
				break;
			}
		}
		if (count != size) colStr += " and ";
	}

	std::string formatSql = "SELECT * FROM %s WHERE %s";
	StatementPtr ptr = executeSql(formatSql.c_str(), tbl->tableName.c_str(), colStr.c_str());
	if (ptr == NULL) {
		return;
	}
	Statement* st = ptr->getStatement();
	sql::ResultSet* rs = st->getResultSet();
	sql::ResultSetMetaData* metaData = rs->getMetaData();
	int colCount = metaData->getColumnCount();
	int iPriKeyIdx = -1;
	for (int i = 1; i <= colCount; i++) {
		std::string colName = metaData->getColumnLabel(i).c_str();
		if (colName == tblSchema->priKeyName) {
			iPriKeyIdx = i;
			break;
		}
	}

	if (iPriKeyIdx < 0) {
		Logger::logError("$not found primary key, table:%s, priKeyName:%s", tbName.c_str(), tblSchema->priKeyName.c_str());
		return;
	}

	std::vector<TableIndex> indexs = tblSchema->tableIndexs;

	while (rs->next()) {
		std::string redisKey;
		std::string redisVal;
		std::map<std::string, std::string> mpColVals;
		Table tblData;
		tblData.tableName = tbl->tableName;
		for (int i = 1; i <= colCount; i++) {
			MAKE_TABLE_FIELD(tbField);
			std::string colName = metaData->getColumnLabel(i).c_str();
			TableField* tbFieldSchema = getTableField(tbl->tableName.c_str(), colName.c_str());
			tbField->fieldName = colName;
			tbField->type = tbFieldSchema->type;
			redisVal += " " + colName + " ";
			int colType = metaData->getColumnType(i);
			if (colType >= sql::DataType::BIT && colType <= sql::DataType::BIGINT) {
				int64_t val = rs->getInt64(i);
				redisVal += val;
				char buf[64];
				snprintf(buf, 64, "%ld", val);
				if (i == iPriKeyIdx) {
					redisKey.append(tbName).append(":").append(buf);
				}
				mpColVals.emplace(colName, buf);
				tbField->lval = val;
			}
			else if (colType >= sql::DataType::REAL && colType <= sql::DataType::NUMERIC) {
				double val = rs->getDouble(i);
				redisVal += val;
				char buf[64];
				snprintf(buf, 64, "%lf", val);
				if (i == iPriKeyIdx) {
					redisKey.append(tbName).append(":").append(buf);
				}
				mpColVals.emplace(colName, buf);
				tbField->dval = val;
			}
			else {
				const char* val = rs->getString(i).c_str();
				redisVal += val;
				if (i == iPriKeyIdx) {
					redisKey.append(tbName).append(":").append(val);
				}
				mpColVals.emplace(colName, val);
				tbField->sval = val;
			}
			tblData.addField(tbField);
		}
		result.push_back(tblData);

		std::string redisCmd = "hmset " + redisKey + " " + redisVal;
		m_redis->execRedisCmd(redisCmd.c_str());

		for (TableIndex tbIndex : tblSchema->tableIndexs) {
			std::string redisIndexKey = tbName + ":" + tbIndex.indexName + ":";
			for (std::string colName : tbIndex.cols) {
				std::string val = mpColVals.find(colName)->second;
				redisIndexKey += val + ":";
			}
			std::string redisCmd = "SADD " + redisIndexKey + " " + redisKey;
			m_redis->execRedisCmd(redisCmd.c_str());
		}
	}
}

bool DBHandler::insertRow(Table* tbl)
{
	std::string fields;
	std::string vals;
	TableSchema* tblSchema = getTableSchema(tbl->tableName.c_str());
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
		TableField* field = iter->second.get();
		fields += fieldName + ",";
		switch (tbField->type) {
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field->lval);
				vals += buf;
				if (isPriKey) tbl->priKeyVal = field->lval;
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field->dval);
				vals += buf;
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				vals += "'" + field->sval + "'";
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

static TableIndex* matchTableIndex(TableSchema* tableSchema, Table* tbl) {
	int iMaxIndexLen = 0;
	TableIndex* tgtIndex = NULL;
	int size = tableSchema->tableIndexs.size();
	for (int i = 0; i < size; i++) {
		TableIndex* tbIndex = &tableSchema->tableIndexs[i];
		int indexLen = 0;
		for (std::string colName : tbIndex->cols) {
			if (tbl->fields.find(colName) != tbl->fields.end()) {
				indexLen++;
			}
		}
		if (indexLen > iMaxIndexLen) {
			tgtIndex = tbIndex;
			iMaxIndexLen = indexLen;
		}
	}
	return tgtIndex;
}

void DBHandler::getRedisKeyValue(std::string& tableName, long keyVal, Table* tblData)
{
	checkRedisExistAndLoad(tableName.c_str(), keyVal);
	std::string redisKey = Table::redisKey(tableName.c_str(), keyVal);
	std::string redisCmd = "hgetall " + redisKey;
	REDIS_REPLY_PTR replyPtr = m_redis->execRedisCmd(redisCmd.c_str());
	redisReply* reply = replyPtr->getReply();
	for (int i = 0; i < reply->elements; i += 2) {
		redisReply* colNameReply = reply->element[i];
		redisReply* colValReply = reply->element[i + 1];
		std::string colName;
		std::copy(colNameReply->str, colNameReply->str + colNameReply->len, std::back_inserter(colName));
		std::string colVal;
		std::copy(colValReply->str, colValReply->str + colValReply->len, std::back_inserter(colVal));
		TableField* tblField = getTableField(tableName.c_str(), colName.c_str());
		MAKE_TABLE_FIELD(field);
		field->fieldName = colName;
		field->type = tblField->type;
		switch (tblField->type)
		{
		case TableField::FieldType::TYPE_INT:
		case TableField::FieldType::TYPE_BIGINT:
			field->lval = atol(colVal.c_str());
			break;
		case TableField::FieldType::TYPE_DOUBLE:
			field->dval = atof(colVal.c_str());
			break;
		case TableField::FieldType::TYPE_VCHAR:
		case TableField::FieldType::TYPE_TEXT:
			field->sval = colVal;
			break;
		default:
			Logger::logError("not support field type:%d, table:%s, col:%s", tblField->type, tableName.c_str(), colName.c_str());
			break;
		}
		tblData->addField(field);
	}
}

bool DBHandler::getRow(Table* tbl, std::vector<Table>& result)
{
	//if (!checkRedisExistAndLoad(tableName, keyVal)) return NULL;
	TableSchema* tableSchema = getTableSchema(tbl->tableName.c_str());
	if (tableSchema == NULL) {
		Logger::logError("get row not found table schema, table:%s", tbl->tableName.c_str());
		return false;
	}

	// 有主键
	TableField* tbField = tbl->getField(tableSchema->priKeyName.c_str());
	if (tbField != NULL) {
		Table tblData;
		getRedisKeyValue(tbl->tableName, tbField->lval, &tblData);
		result.push_back(tblData);
		return true;
	}

	TableIndex* tgtIndex = matchTableIndex(tableSchema, tbl);
	// 匹配到索引
	if (tgtIndex != NULL) {
		std::string redisIndexKey = tbl->tableName + ":" + tgtIndex->indexName + ":";
		for (std::string colName : tgtIndex->cols) {
			TableField* field = tbl->getField(colName.c_str());
			TableField* tbField = getTableField(tbl->tableName.c_str(), field->fieldName.c_str());
			switch (tbField->type)
			{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field->lval);
				redisIndexKey += buf;
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field->lval);
				redisIndexKey += buf;
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				redisIndexKey += field->sval;
				break;
			}
			default:
			{
				Logger::logError("$not support field type:%d, table:%s, col:%s", tbField->type, tbl->tableName.c_str(), field->fieldName.c_str());
				break;
			}
			}
			redisIndexKey += ":";
		}

		REDIS_REPLY_PTR replyPtr = m_redis->execRedisCmd("EXISTS %s", redisIndexKey.c_str());
		if (replyPtr->getReply()->integer == 1) {
			std::string redisCmd = "SMEMBERS " + redisIndexKey;
			REDIS_REPLY_PTR replyPtr = m_redis->execRedisCmd(redisCmd.c_str());
			redisReply* reply = replyPtr->getReply();
			if (reply->type != REDIS_REPLY_ARRAY) {
				return false;
			}

			for (int i = 0; i < reply->elements; i++) {
				redisReply* subReply = reply->element[i];
				std::string s;
				std::copy(subReply->str, subReply->str + subReply->len, std::back_inserter(s));
				int npos = s.find_first_of(':');
				long lval = atol(s.substr(npos + 1, s.size() - npos).c_str());

				Table tblData;
				getRedisKeyValue(tbl->tableName, lval, &tblData);
				result.push_back(tblData);
			}
			return true;
		}
	}

	// 没有索引, 从DB加载
	loadFromDB(tbl, result);
	Logger::logWarning("$not match index when get data, you need create index on table %s", tbl->tableName.c_str());
	return true;
}

void getEffectIndex(std::map<std::string, std::shared_ptr<TableField>>& fields, std::vector<TableIndex>& indexs, std::vector<TableIndex*>* effectIndexs) {
	for (int i = 0; i < indexs.size(); i++) {
		for (std::string col : indexs[i].cols) {
			if (fields.find(col) != fields.end()) {
				effectIndexs->emplace_back(&indexs[i]);
				break;
			}
		}
	}
}

bool DBHandler::updateRow(Table* tbl)
{
	if (!checkRedisExistAndLoad(tbl->tableName.c_str(), tbl->priKeyVal)) return false;
	Table tblData;
	getRedisKeyValue(tbl->tableName, tbl->priKeyVal, &tblData);
	std::string redisCmd = "hmset " + tbl->redisKey();
	std::set<std::string> stFields;
	TableSchema* tableSchema = getTableSchema(tbl->tableName.c_str());

	for (auto iter = tbl->fields.begin(); iter != tbl->fields.end(); iter++) {
		std::string fieldName = iter->first;

		stFields.insert(fieldName);
		TableField* field = iter->second.get();
		redisCmd.append(" " + fieldName + " ");
		switch (field->type) {
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field->lval);
				redisCmd.append(buf);
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field->dval);
				redisCmd.append(buf);
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				redisCmd.append("'" + field->sval + "'");
				break;
			}
			default:
				Logger::logError("$not support field type, table:%s, field:%s, type:%d", tbl->tableName.c_str(), fieldName.c_str(), field->type);
				return false;
		}
	}
	m_redis->execRedisCmd("MULTI");
	m_redis->execRedisCmd(redisCmd.c_str());


	std::vector<TableIndex*> effectIndexs;
	getEffectIndex(tbl->fields, tableSchema->tableIndexs, &effectIndexs);
	for (TableIndex* tbIndex : effectIndexs) {
		std::string oldIndexKey = tbl->tableName + ":" + tbIndex->indexName + ":";
		std::string newIndexKey = tbl->tableName + ":" + tbIndex->indexName + ":";
		for (std::string col : tbIndex->cols) {
			TableField* fieldSchema = tableSchema->getFieldSchema(col.c_str());
			TableField* newField = tbl->getField(col.c_str());
			TableField* oldField = tblData.getField(col.c_str());
			switch (fieldSchema->type) {
				case TableField::FieldType::TYPE_INT:
				case TableField::FieldType::TYPE_BIGINT:
				{
					char buf[64]{ 0 };
					snprintf(buf, 64, "%ld", oldField->lval);
					oldIndexKey.append(buf);

					long lval = newField != NULL ? newField->lval : oldField->lval;
					snprintf(buf, 64, "%ld", lval);
					newIndexKey.append(buf);
					break;
				}
				case TableField::FieldType::TYPE_DOUBLE:
				{
					char buf[64]{ 0 };
					snprintf(buf, 64, "%lf", oldField->dval);
					oldIndexKey.append(buf);

					double dval = newField != NULL ? newField->dval : oldField->dval;
					snprintf(buf, 64, "%lf", dval);
					newIndexKey.append(buf);
					break;
				}
				case TableField::FieldType::TYPE_VCHAR:
				case TableField::FieldType::TYPE_TEXT:
				{
					oldIndexKey.append("'" + oldField->sval + "'");
					std::string sval = newField != NULL ? newField->sval : oldField->sval;
					newIndexKey.append("'" + sval + "'");
					break;
				}
			}
			
		}

		m_redis->execRedisCmd("SREM %s %s", oldIndexKey.c_str(), tbl->redisKey());
		m_redis->execRedisCmd("SADD %s %s", newIndexKey.c_str(), tbl->redisKey());
	}
	REDIS_REPLY_PTR ptr = m_redis->execRedisCmd("EXEC");
	if (ptr == NULL || ptr->getReply()->type == REDIS_REPLY_NIL) {
		Logger::logInfo("$update row failed!! table:%s", tbl->tableName.c_str());
		return false;
	}

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
