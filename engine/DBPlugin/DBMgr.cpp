#include "DBMgr.h"

#include <iterator>
#include <exception>

#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"

#include "Logger.h"

#ifdef WIN32
#include "mongo/MongoDBHandler.h"
#endif

INIT_SINGLETON_CLASS(DBMgr)

DBMgr::DBMgr(std::string& dbUserName, std::string& dbPassword, std::string dbUrl, int dbPort) : 
	m_dbUserName(dbUserName), m_dbPassword(dbPassword), m_dbUrl(dbUrl), m_dbPort(dbPort)
{
	
}

DBHandler* DBMgr::createDBHander(const char* dbName) {
	DBMgr* dbMgr = DBMgr::getSingleton();
	if (dbMgr == NULL) {
		return NULL;
	}
	auto iter = dbMgr->m_dbHanders.find(dbName);
	if (iter != dbMgr->m_dbHanders.end()) {
		return iter->second;
	}
	DBHandler* dbHanler = new DBHandler(m_dbUrl.c_str(), m_dbPort, m_dbUserName.c_str(), m_dbPassword.c_str(), dbName);
	dbMgr->m_dbHanders.emplace(std::make_pair(dbName, dbHanler));
	
#ifdef WIN32
	std::string mdb_uri = "mongodb://127.0.0.1:27017/?appname=client-example";
	std::string mdb_name = "test";
	MongoDBHandler mdb(mdb_uri, mdb_name);
#endif

	return dbHanler;
}

DBHandler* DBMgr::getDBHander(const char* dbName) 
{
	DBMgr* dbMgr = DBMgr::getSingleton();
	if (dbMgr == NULL) {
		return NULL;
	}
	auto iter = dbMgr->m_dbHanders.find(dbName);
	if (iter != dbMgr->m_dbHanders.end()) {
		return iter->second;
	}
	return NULL;
}

