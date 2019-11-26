#pragma once
#include <vector>
#include <Reflect.h>

#ifdef DBPLUGIN_EXPORT
#define DBPLUNGIN_API __declspec(dllexport)
#else
#define DBPLUNGIN_API
#endif

class DBPLUNGIN_API DBHandler
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
	void executeSql(std::string sql);
};