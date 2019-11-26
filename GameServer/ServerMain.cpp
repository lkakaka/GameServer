
#include <iostream>
#include <thread>

#include "boost/asio.hpp"

#include "Network.h"
//#include "Log.h"
#include "DBPlugin.h"
#include "Logger.h"
#include "DBTableDef.h"
//#include "DBMgr.h"
#include "PythonPlugin.h"
#include "ZmqInst.h"
#include "Timer.h"
#include "UnitTest.h"

using namespace std;

int main()
{
	Logger::initLog();

	boost::asio::io_service io;
	TimerMgr::initTimerMgr(&io);

	initPython();

	/*DBPlugin* dbPlugin = new DBPlugin();
	dbPlugin->initDBPlugin("");*/

	/*DBMgr* dbMgr = new DBMgr("test");
	TblPlayer* tblPlayer = new TblPlayer();
	std::vector<ReflectObject*> tbls;
	tbls.push_back((ReflectObject*)tblPlayer);
	dbMgr->initDbTable(tbls);*/
	//DBMgr::getDBMgrInstance();

	Network* network = new Network(&io);
	network->startListen();

	ZmqInst* zmq = new ZmqInst();
	zmq->startZmqInst();
	
	UnitTest::test();

	Logger::logInfo("$MyServer Start!!!");

	io.run();

	finalizePython();
	Logger::logInfo("$MyServer exit!!!");

	return 0;
}