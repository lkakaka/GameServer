#pragma once
#include <vector>
#include <Reflect.h>
#include "DBHandler.h"
#include "Singleton.h"

using namespace DataBase;

class SERVER_EXPORT_API DBMgr : public Singleton<DBMgr>
{
private:
	std::string m_dbUserName;
	std::string m_dbPassword;
	std::string m_dbUrl;
	int m_dbPort;
	std::map<std::string, DBHandler*> m_dbHanders;
public:

	DBMgr(std::string& dbUserName, std::string& dbPassword, std::string dbUrl, int dbPort);

	DBHandler* createDBHander(char* dbName);
	DBHandler* getDBHander(char* dbName);
};



