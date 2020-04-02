#pragma once
#include <vector>
#include <Reflect.h>
#include <functional>
#include <vector>
#include "../Common/ServerExports.h"
#include "jdbc/cppconn/statement.h"

class SERVER_EXPORT_API DBHandler
{
private:
	std::string m_dbName;
	void createTable(ReflectObject* tbl);
	//sql::Connection* getDBConnection();

public:
	DBHandler(std::string dbName);
	inline std::string getDbName();
	void initDbTable(std::vector<ReflectObject*> tblDefs);

	void insert(std::vector<ReflectObject> data);
	void insertOne(ReflectObject tbl);
	void select(ReflectObject data);
	void update(ReflectObject src, ReflectObject dst);
	void del(ReflectObject data);
	void executeSql(std::string sql, std::function<void(sql::Statement*, bool) > handler);
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