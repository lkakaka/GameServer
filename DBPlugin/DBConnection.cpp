#include "DBConnection.h"


DBHandler::DBHandler(std::string dbUrl): m_dbUrl(dbUrl), m_conn(NULL)
{

}

void DBHandler::connect(std::string hostname, int port, std::string user, std::string pwd, std::string dbname)
{	
	sql::Driver* driver = get_driver_instance();
	m_conn = driver->connect("tcp://127.0.0.1:3306/test1", "root", "");
}

std::vector<ReflectObject> DBHandler::select(ReflectObject in)
{
	std::map<std::string, Field> fieldMap = in.getFieldMap();

}