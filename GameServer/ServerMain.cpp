
#include <iostream>
#include <thread>

#include "boost/asio.hpp"

#include "Network.h"
//#include "Log.h"
#include "DBPlugin.h"
#include "Logger.h"
#include "DBTableDef.h"
#include "DBMgr.h"
#include "PythonPlugin.h"
#include "../Math/MathFunction.h"

using namespace std;

#pragma comment(lib, "MathFunction.lib")

void threadFunc()
{
	Logger::logInfo("$Thread Start");
	callPyFunction("main", "thread_test");
	Logger::logInfo("$Thread End");
}

int main()
{
	Logger::initLog();

	initPython();

	/*DBPlugin* dbPlugin = new DBPlugin();
	dbPlugin->initDBPlugin("");*/

	DBMgr* dbMgr = new DBMgr("test");
	TblPlayer* tblPlayer = new TblPlayer();
	std::vector<ReflectObject*> tbls;
	tbls.push_back((ReflectObject*)tblPlayer);
	dbMgr->initDbTable(tbls);

	boost::asio::io_service io;
	Network* network = new Network(&io);
	network->startListen();
	
	double x = power(2, 3);
	/*int a;
	printf("ptr size is %d", sizeof(a));*/

	/*boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
	t.wait();*/

	std::thread t(threadFunc);
	t.join();
	
	Logger::logInfo("$MyServer Start!!!");

	io.run();

	Logger::logInfo("$MyServer exit!!!");

	finalizePython();

	return 0;
}