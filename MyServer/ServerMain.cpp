
#include <iostream>
#include "boost/asio.hpp"

#include "Network.h"
//#include "Log.h"
#include "DBPlugin.h"
#include "Logger.h"
#include "DBTableDef.h"
#include "DBMgr.h"

using namespace std;

int main()
{
	Logger::initLog();

	DBPlugin* dbPlugin = new DBPlugin();
	dbPlugin->initDBPlugin("");

	/*DBMgr* dbMgr = new DBMgr("test");
	TblPlayer* tblPlayer = new TblPlayer();
	std::vector<ReflectObject*> tbls;
	tbls.push_back((ReflectObject*)tblPlayer);
	dbMgr->initDbTable(tbls);*/

	boost::asio::io_service io;
	Network* network = new Network(&io);
	network->startListen();

	/*int a;
	printf("ptr size is %d", sizeof(a));*/

	/*boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
	t.wait();*/
	
	Logger::logInfo("$MyServer Start!!!");

	io.run();

	return 0;
}