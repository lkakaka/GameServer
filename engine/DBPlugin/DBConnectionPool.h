#pragma once
#include <string>
#include <vector>

#include "jdbc/cppconn/driver.h"
//#include "jdbc/cppconn/resultset.h"

class DBConnectionPool
{
private:
	std::string m_jdbcUrl;
	std::vector<sql::Connection*> m_connPool;
	std::vector<sql::Connection*> m_usedConnPool;
	int m_maxConnCount = 3;
};

