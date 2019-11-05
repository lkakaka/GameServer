#include "DBPlugin.h"
#include "Reflect.h"
#include "jdbc/cppconn/driver.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"


using namespace sql;

void DBPlugin::initDBPlugin(std::string dbDir)
{
	Logger::initLog();

	RelectTest test;
	test.setInt("id", 1);
	test.setString("name", "hello");

	Logger::logInfo("$relect :%d, %s", test.id, test.name.c_str());

	try {
		Driver* driver = get_driver_instance();
		Connection* m_dbConn = driver->connect("tcp://127.0.0.1:3306/test1", "root", "");
		if (m_dbConn == NULL) {
			Logger::logError("$connect mysql failed");
			return;
		}
		//conn->setSchema("test1");

		Statement* st = m_dbConn->createStatement();
		std::string sql = "CREATE TABLE IF NOT EXISTS TblPlayer(id INT)";
		sql::SQLString sqlStr = sql::SQLString(sql.c_str());
		//sql::SQLString sqlStr = sql;
		st->execute(sqlStr);

		/*Statement* stmt = conn->createStatement();
		stmt->executeQuery("SELECT * FROM bd_pred_dut");*/

		std::unique_ptr<Statement> stmt(nullptr);
		//Statement stmt; 
		stmt.reset(m_dbConn->createStatement());
		/* run a query which returns exactly one result set */
		std::unique_ptr<ResultSet> res(nullptr);
		res.reset(stmt->executeQuery("SELECT name FROM player"));

		ResultSetMetaData* metaData = res->getMetaData();
		for (int i = 1; i <= metaData->getColumnCount(); i++) {
			int type = metaData->getColumnType(i);
			Logger::logInfo("$colname:%s, type:%d", metaData->getColumnName(i).c_str(), type);
		}

		/* retrieve the data from the result set and display on stdout */
		/* retrieve the row count in the result set */
		std::cout << "\nRetrieved " << res->rowsCount() << " row(s)." << std::endl;
		/* fetch the data : retrieve all the rows in the result set */
		while (res->next()) {
			/*int id = res->getInt(1);
			std::cout << id << std::endl;*/
			SQLString str = res->getString("name");
			std::cout << str.c_str() << std::endl;
			//std::cout << res->getString("name") << std::endl;
		} // if-else

		std::cout << std::endl;
	}
	catch (std::exception e) {
		Logger::logError("$sql error %s", e.what());
	}

	Logger::logInfo("$init db plugin");
}
