#pragma once
#include <string>
#include <vector>
#include "Reflect.h"
#include "jdbc/cppconn/driver.h" 

class DBHandler {
private:
	std::string m_dbUrl;
	sql::Connection* m_conn;
	void connect(std::string hostname, int port, std::string user, std::string pwd, std::string dbname);
public:
	DBHandler(std::string dbUrl);

	std::vector<ReflectObject> select(ReflectObject in);
	std::vector<ReflectObject> update(ReflectObject in);
	std::vector<ReflectObject> alter(ReflectObject in);
	std::vector<ReflectObject> del(ReflectObject in);
};