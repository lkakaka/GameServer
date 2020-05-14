#pragma once
#include <vector>
#include <Reflect.h>
#include <functional>
#include <memory>
#include "../Common/ServerExports.h"
#include "jdbc/cppconn/statement.h"

class SERVER_EXPORT_API DBHandler
{
private:
	std::string m_dbUrl;
	int m_dbPort;
	std::string m_dbUserName;
	std::string m_dbPassword;
	std::string m_dbName;
	sql::Connection* m_dbConn;

	void createTable(ReflectObject* tbl);
	sql::Connection* getDBConnection();

public:
	DBHandler(std::string& dbUrl, int dbPort, std::string& dbUserName, std::string& dbPassword, std::string dbName);
	~DBHandler();
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
	std::string name;	// ×Ö¶ÎÃû
	int type;			// ×Ö¶ÎÀàÐÍ£¨FieldType£©
	void* val;			// ×Ö¶ÎµØÖ·
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
