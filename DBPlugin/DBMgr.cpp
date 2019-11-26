#include "DBMgr.h"

#include <iterator>
#include <exception>

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"

#include "Logger.h"

DBMgr* g_DBMgr;

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
	DBHandler* dbHanler = new DBHandler(dbName);
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

