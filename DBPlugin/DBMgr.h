#pragma once
#include <vector>
#include <Reflect.h>
#include "DBHandler.h"

using namespace DataBase;

class SERVER_EXPORT_API DBMgr
{
private:
	std::string m_dbName;
	std::map<std::string, DBHandler*> m_dbHanders;
public:

	DBMgr();
	static std::string m_dbUserName;
	static std::string m_dbPassword;
	static std::string m_dbUrl;
	static int m_dbPort;
	static DBMgr* getDBMgrInstance();
	static DBHandler* createDBHander(char* dbName);
	static DBHandler* getDBHander(char* dbName);
};

