#pragma once
#include <vector>
#include <Reflect.h>
#include "DBHandler.h"

class DBPLUNGIN_API DBMgr
{
private:
	std::string m_dbName;
	std::map<std::string, DBHandler*> m_dbHanders;

public:

	DBMgr();
	static DBMgr* getDBMgrInstance();
	static DBHandler* createDBHander(char* dbName);
	static DBHandler* getDBHander(char* dbName);
};

