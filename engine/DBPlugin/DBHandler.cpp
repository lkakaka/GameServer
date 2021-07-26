#include "DBHandler.h"

#include <iterator>
#include <exception>

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/datatype.h"
#include "jdbc/cppconn/prepared_statement.h"

#include "Logger.h"

#include "mysqlx/xdevapi.h"
#include <codecvt>

using namespace sql;
//using namespace mysqlx;

USING_DATA_BASE;


DBHandler::~DBHandler()
{
	Logger::logInfo("$db hander destory!!!");
}

DBHandler::DBHandler(const char* dbUrl, int dbPort, const char* dbUserName, const char* dbPassword, const char* dbName) :
	m_dbUrl(dbUrl), m_dbPort(dbPort), m_dbUserName(dbUserName), m_dbPassword(dbPassword), m_dbName(dbName), m_dbConn(NULL),
	m_isCreateDB(false)
{
	Connection* conn = getDBConnection();
	Statement* st = conn->createStatement();
	std::string sql = "CREATE DATABASE IF NOT EXISTS ";
	sql += dbName;
	sql::SQLString sqlStr = sql::SQLString(sql.c_str());
	st->execute(sqlStr);
	conn->close();
	m_isCreateDB = true;
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

	// Connect to server
	//mysqlx::Session session("localhost", dbPort, dbUserName, dbPassword);
#ifdef USE_MYSQLX
	/*mysqlx::SessionSettings settings(mysqlx::SessionOption::HOST, "localhost",
		mysqlx::SessionOption::PORT, dbPort);*/
	
		//std::string url = "mysqlx://localhost:3306/test?user=game_dev&password=123456";
		//mysqlx::Session session1("localhost", dbPort, dbUserName, dbPassword);
		//const char* url = ("mysqlx://sa:xxxxxxxx@127.0.0.1/sakila");

		//mysqlx::string userName(dbUserName);
		//std::copy(dbUserName.begin(), dbUserName.end(), std::back_inserter(userName));
		//mysqlx::string pwd(to_utf16(dbPassword));
		//std::copy(dbPassword.begin(), dbPassword.end(), std::back_inserter(pwd));
	
		mysqlx::SessionSettings settings("localhost", 33060, dbUserName, dbPassword);

		/*settings.set(mysqlx::SessionOption::USER, dbUserName);
		settings.set(mysqlx::SessionOption::PWD, dbPassword);*/

		mysqlx::Session session(settings);

		mysqlx::Schema db = session.getSchema(dbName);

		// Create a new collection 'my_collection'
		mysqlx::Collection myColl = db.createCollection("my_collection");

		// Insert documents
		myColl.add(R"({"name": "Laurie", "age": 19})").execute();
		myColl.add(R"({"name": "Nadya", "age": 54})").execute();
		myColl.add(R"({"name": "Lukas", "age": 32})").execute();

		// Find a document
		mysqlx::DocResult docs = myColl.find("name like :param1 AND age < :param2").limit(1)
			.bind("param1", "L%").bind("param2", 20).execute();

		// Print document
		std::cout << docs.fetchOne() << std::endl;

		// Drop the collection
		db.dropCollection("my_collection");
#endif
}

void DBHandler::initTableSchema() {
	m_tableSchema.clear();
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
		/*sql = "select * from(SELECT a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name, GROUP_CONCAT(column_name ORDER BY seq_in_index) AS `columns` "
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
		}*/


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
	if (m_isCreateDB && !m_dbName.empty())
	{
		url += "/" + m_dbName;
	}
	sql::SQLString sqlUrl = sql::SQLString(url.c_str());
	sql::SQLString dbUserName = sql::SQLString(m_dbUserName.c_str());
	sql::SQLString dbPassword = sql::SQLString(m_dbPassword.c_str());
	sql::Connection* conn = driver->connect(sqlUrl, dbUserName, dbPassword);
	if (m_isCreateDB) m_dbConn = conn;
	return conn;
}


bool isSameCol(TableField* col1, TableField* col2) {
	if (col1->fieldName != col2->fieldName) return false;
	if (col1->type != col2->type) return false;
	if (col1->length != col2->length) return false;
	if (col1->defaut_val != col2->defaut_val) return false;
	return true;
}

bool DBHandler::initTable(Table* tbl)
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
		"GROUP BY a.TABLE_SCHEMA, a.TABLE_NAME, a.index_name) as b where b.TABLE_NAME = '%s' and b.TABLE_SCHEMA = '%s' and b.index_name LIKE 'Index_%%'";
	ptr = executeSql(sql, tbName.c_str(), m_dbName.c_str());
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
			if (tbl->isAutoIncr && (field->type == TableField::FieldType::TYPE_BIGINT || TableField::FieldType::TYPE_INT)) {
				colStr += " AUTO_INCREMENT";
			}
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
	bool isFirstColChg = true;
	std::string chgColStr;
	for (int col = 0; col < colNum; col++) {
		std::string colName = tbl->colNames[col];
		TableField* field = tbl->getField(colName.c_str()); //tbl->fields.find(colName)->second;
		
		std::string colStr;
		if (field->isDel) { // 删除字段
			if (orgFields.find(field->fieldName) == orgFields.end()) {
				Logger::logWarning("$delete column %s in table %s not found", field->fieldName.c_str(), tbName.c_str());
				continue;
			}
			colStr = "DROP " + colName;
		} else { // 修改字段
			if (field->oldName == "") {
				auto iter = orgFields.find(colName);
				if (iter == orgFields.end()) { // 新加字段
					colStr = "ADD ";
				}
				else {  // 修改字段
					if (isSameCol(field, &iter->second)) {
						continue;
					}
					colStr = "MODIFY ";
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
				colStr = "CHANGE " + field->oldName + " ";
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

		if (isFirstColChg) {
			chgColStr += colStr;
			isFirstColChg = false;
		}
		else {
			chgColStr += "," + colStr;
		}
		if (!isChange) isChange = true;
	}

	if (!isChange) return true;

	char* sql = "ALTER TABLE %s %s";
	StatementPtr ptr = executeSql(sql, tbName.c_str(), chgColStr.c_str());
	if (ptr == NULL) {
		Logger::logError("$alter table %s failed", tbName.c_str());
		return false;
	}
	Logger::logInfo("$alter table %s, %s", tbName.c_str(), chgColStr.c_str());
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

bool DBHandler::loadFromDB(Table* tbl, std::vector<Table>& result) {
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
		return false;
	}
	Statement* st = ptr->getStatement();
	sql::ResultSet* rs = st->getResultSet();
	sql::ResultSetMetaData* metaData = rs->getMetaData();
	int colCount = metaData->getColumnCount();

	while (rs->next()) {
		std::map<std::string, std::string> mpColVals;
		Table tblData;
		tblData.tableName = tbl->tableName;
		for (int i = 1; i <= colCount; i++) {
			MAKE_TABLE_FIELD(tbField);
			std::string colName = metaData->getColumnLabel(i).c_str();
			TableField* tbFieldSchema = getTableField(tbl->tableName.c_str(), colName.c_str());
			tbField->fieldName = colName;
			tbField->type = tbFieldSchema->type;
			int colType = metaData->getColumnType(i);
			if (colType >= sql::DataType::BIT && colType <= sql::DataType::BIGINT) {
				int64_t val = rs->getInt64(i);
				char buf[64];
				snprintf(buf, 64, "%ld", val);
				mpColVals.emplace(colName, buf);
				tbField->lval = val;
			}
			else if (colType >= sql::DataType::REAL && colType <= sql::DataType::NUMERIC) {
				double val = rs->getDouble(i);
				char buf[64];
				snprintf(buf, 64, "%lf", val);
				mpColVals.emplace(colName, buf);
				tbField->dval = val;
			}
			else {
				SQLString sqlStr = rs->getString(i);
				const char* val = sqlStr.c_str();
				mpColVals.emplace(colName, val);
				tbField->sval = val;
			}
			tblData.addField(tbField);
		}
		result.push_back(tblData);
	}
	return true;
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

bool DBHandler::getRow(Table* tbl, std::vector<Table>& result)
{
	//if (!checkRedisExistAndLoad(tableName, keyVal)) return NULL;
	TableSchema* tableSchema = getTableSchema(tbl->tableName.c_str());
	if (tableSchema == NULL) {
		Logger::logError("get row not found table schema, table:%s", tbl->tableName.c_str());
		return false;
	}
	return loadFromDB(tbl, result);
}

bool DBHandler::updateRow(Table* tbl)
{
	TableSchema* tableSchema = getTableSchema(tbl->tableName.c_str());

	std::string chg_fields;
	std::string conditions;

	for (auto iter = tbl->fields.begin(); iter != tbl->fields.end(); iter++) {
		std::string fieldName = iter->first;
		TableField* field = iter->second.get();
		bool isPriKey = tableSchema->priKeyName == field->fieldName;
		switch (field->type) {
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field->lval);
				if (isPriKey) {
					conditions = field->fieldName + "=" + buf;
				}
				else {
					chg_fields += field->fieldName + "=" + buf;
				}
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field->dval);
				if (isPriKey) {
					conditions = field->fieldName + "=" + buf;
				}
				else {
					chg_fields += field->fieldName + "=" + buf;
				}
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				if (isPriKey) {
					conditions = field->fieldName + "='" + field->sval + "'";
				}
				else {
					chg_fields += field->fieldName + "='" + field->sval + "'";
				}
				break;
			}
			default:
				Logger::logError("$not support field type, table:%s, field:%s, type:%d", tbl->tableName.c_str(), fieldName.c_str(), field->type);
				return false;
		}
		if (!isPriKey) {
			chg_fields += ",";
		}
	}

	// 删除最后的逗号
	if (!chg_fields.empty()) {
		chg_fields.pop_back();
	}


	char* sql = "UPDATE %s SET %s WHERE %s";
	StatementPtr ptr = executeSql(sql, tbl->tableName.c_str(), chg_fields.c_str(), conditions.c_str());
	if (ptr == NULL) {
		Logger::logError("$exe update sql failed, sql:%s, fields:%s, conditions:%s", sql, chg_fields.c_str(), conditions.c_str());
		return false;
	}
	return true;
}

bool DBHandler::replaceRow(Table* tbl)
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

		TableField* field = iter->second.get();
		fields += fieldName + ",";
		switch (tbField->type) {
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field->lval);
				vals += buf;
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

	char* sql = "REPLACE INTO %s(%s) VALUES(%s)";
	StatementPtr ptr = executeSql(sql, tbl->tableName.c_str(), fields.c_str(), vals.c_str());
	if (ptr == NULL) {
		Logger::logError("$exe replace sql failed, sql:%s, fields:%s, vals:%s", sql, fields.c_str(), vals.c_str());
		return false;
	}

	return true;
}

std::string formatSqlConditions(Table* tbl) {
	std::string formatStr;
	int size = tbl->fields.size();
	int count = 0;
	for (auto iter = tbl->fields.begin(); iter != tbl->fields.end(); iter++) {
		count++;
		std::string fieldName = iter->first;
		TableField* field = iter->second.get();
		formatStr += fieldName + "=";
		switch (field->type) {
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%ld", field->lval);
				formatStr += buf;
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				char buf[64]{ 0 };
				snprintf(buf, 64, "%lf", field->dval);
				formatStr += buf;
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				formatStr += "'" + field->sval + "'";
				break;
			}
			default:
				Logger::logError("$not support field type, table:%s, field:%s, type:%d", tbl->tableName.c_str(), fieldName.c_str(), field->type);
				break;
		}

		if (count != size) formatStr += " AND ";
	}
	return formatStr;
}

bool DBHandler::deleteRow(Table* tbl)
{
	if (tbl->fields.empty()) {
		Logger::logError("$delete row not set conditions");
		return false;
	}
	std::string cond = formatSqlConditions(tbl);
	if (executeSql("DELETE FROM %s WHERE %s", tbl->tableName.c_str(), cond.c_str()) == NULL)
		return false;
	return true;
}
