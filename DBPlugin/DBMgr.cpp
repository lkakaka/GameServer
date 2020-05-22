#include "DBMgr.h"

#include <iterator>
#include <exception>

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"

#include "Logger.h"


DBMgr* g_DBMgr;

std::string DBMgr::m_dbUserName = "";
std::string DBMgr::m_dbPassword = "";
std::string DBMgr::m_dbUrl = "";
int DBMgr::m_dbPort = 0;

DBMgr* DBMgr::getDBMgrInstance()
{
	if (g_DBMgr == NULL)
	{
		g_DBMgr = new DBMgr();
	}
	return g_DBMgr;
}

DBMgr::DBMgr()
{
	
}

DBHandler* DBMgr::createDBHander(char* dbName) {
	DBMgr* dbMgr = DBMgr::getDBMgrInstance();
	if (dbMgr == NULL) {
		return NULL;
	}
	auto iter = dbMgr->m_dbHanders.find(dbName);
	if (iter != dbMgr->m_dbHanders.end()) {
		return iter->second;
	}
	DBHandler* dbHanler = new DBHandler(DBMgr::m_dbUrl, DBMgr::m_dbPort, DBMgr::m_dbUserName, DBMgr::m_dbPassword, dbName);
	dbMgr->m_dbHanders.emplace(std::make_pair(dbName, dbHanler));
	return dbHanler;
}

DBHandler* DBMgr::getDBHander(char* dbName) 
{
	DBMgr* dbMgr = DBMgr::getDBMgrInstance();
	if (dbMgr == NULL) {
		return NULL;
	}
	auto iter = dbMgr->m_dbHanders.find(dbName);
	if (iter != dbMgr->m_dbHanders.end()) {
		return iter->second;
	}
	return NULL;
}

