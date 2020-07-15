#pragma once
#include <vector>
#include <set>
#include <Reflect.h>
#include <functional>
#include <memory>
#include "../Common/ServerExports.h"
#include "jdbc/cppconn/statement.h"
#include "Redis.h"
#include "DataBase.h"
#include "DBTable.h"

DATA_BASE_BEGIN

#define MAX_SQL_LENGTH (4 * 1024)  // sql语句长度

class _Statement
{
private:
	sql::Statement* m_st;
	bool m_isResultSet;
public:
	_Statement(sql::Statement* st, bool isResultSet) : m_st(st), m_isResultSet(isResultSet){
		//printf("statement constructor\n");
	}
	~_Statement() {
		//printf("statement free\n");
		if (m_st != NULL) m_st->close();
	}
	inline sql::Statement* getStatement() { return m_st; }
	inline bool isResultSet() { return m_isResultSet; }
};

typedef std::shared_ptr<_Statement> StatementPtr;
#define MAKE_STATEMENT_PTR(ptr, isResultSet) std::make_shared<_Statement>(ptr, isResultSet)


class SERVER_EXPORT_API DBHandler
{
private:
	std::string m_dbUrl;
	int m_dbPort;
	std::string m_dbUserName;
	std::string m_dbPassword;
	std::string m_dbName;
	sql::Connection* m_dbConn;
	char m_sqlBuf[MAX_SQL_LENGTH]{ 0 };

	std::map<std::string, std::shared_ptr<TableSchema>> m_tableSchema;

	std::map<std::string, std::map<long, std::set<std::string>>> m_chgRecords; // 已经变化了记录

	//void createTable(ReflectObject* tbl);
	sql::Connection* getDBConnection();

	void loadFromDB(Table* tbl, std::vector<Table>& result);

	void getTableCols(const char* tableName, std::vector<TableField>& fields);
	bool _createNewTable(Table* tbl);
	bool _changeTable(Table* tbl, std::map<std::string, TableField>& orgFields);

public:
	DBHandler(std::string& dbUrl, int dbPort, std::string& dbUserName, std::string& dbPassword, std::string dbName);
	~DBHandler();
	inline std::string getDbName();
	void initDbTable(std::vector<ReflectObject*> tblDefs);

	void initTableSchema();

	TableField* getTableField(const char* tableName, const char* fieldName) const;
	TableSchema* getTableSchema(const char* tableName) const;

	bool initTable(Table* tbl);

	void insert(std::vector<ReflectObject> data);
	void insertOne(ReflectObject tbl);
	void select(ReflectObject data);
	void update(ReflectObject src, ReflectObject dst);
	void del(ReflectObject data);
	StatementPtr executeSql(const char* sqlFromat, ...);

	bool insertRow(Table* tbl);
	bool getRow(Table* tbl, std::vector<Table>& result);
	bool updateRow(Table* tbl);
	bool replaceRow(Table* tbl);
	bool deleteRow(Table* tbl);
};

class DBField
{
public:
	std::string name;	// 字段名
	int type;			// 字段类型（FieldType）
	void* val;			// 字段地址
};

class DBRow {
	std::vector<std::string> m_cols;
	std::vector<void*> m_dat;
	DBRow* next;
};

class DBResult {
public:
	int rsType;
	int updateCount;
	DBRow* head;
};

DATA_BASE_END
