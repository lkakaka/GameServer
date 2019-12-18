
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
#include "DBPlugin.h"
#include "Logger.h"
#include "DBTableDef.h"
//#include "DBMgr.h"
#include "PythonPlugin.h"
#include "ZmqInst.h"
#include "Timer.h"
#include "UnitTest.h"
#include "Config.h"
#include "ProtoBufferMgr.h"

using namespace std;

int main(int argc, char** argv)
{
	Logger::initLog();

	if (argc < 2) {
		Logger::logError("$arg count error");
		return 0;
	}
	char* cfgName = argv[1];
	if (!Config::checkFileExist(cfgName)) {
		Logger::logError("$cfg file not exist, file name: %s", cfgName);
		return 0;
	}

	std::string zmqName = Config::getConfigStr(cfgName, "zmq_name");
	if (zmqName.length() == 0) {
		Logger::logError("$not config zmq name, file name: %s", cfgName);
		return 0;
	}
	std::string routerAddr = Config::getConfigStr(cfgName, "router_addr");
	if (routerAddr.length() == 0) {
		Logger::logError("$not config router addr, file name: %s", cfgName);
		return 0;
	}

	boost::asio::io_service io;
	TimerMgr::initTimerMgr(&io);

	initPython();

	ZmqInst::initZmqInstance(zmqName.c_str(), routerAddr.c_str());
	ZmqInst::getZmqInstance()->setRecvCallback(ProtoBufferMgr::onRecvData);

	/*DBPlugin* dbPlugin = new DBPlugin();
	dbPlugin->initDBPlugin("");*/

	/*DBMgr* dbMgr = new DBMgr("test");
	TblPlayer* tblPlayer = new TblPlayer();
	std::vector<ReflectObject*> tbls;
	tbls.push_back((ReflectObject*)tblPlayer);
	dbMgr->initDbTable(tbls);*/
	//DBMgr::getDBMgrInstance();

	//Network::initNetwork(&io);
//	int port = Config::getConfigInt(cfgName, "port");
//	if (port > 0) {
//#ifdef _DEBUG
//		HINSTANCE h = LoadLibrary("Gateway_d.dll");
//#else
//		HINSTANCE h = LoadLibrary("Gateway.dll");
//#endif // _DEBUG
//		if (h == NULL) {
//			Logger::logError("$gateway.dll not found");
//			return 1;
//		}
//		typedef void(*FunPtr)(boost::asio::io_service*, int);//定义函数指针
//		FunPtr funPtr = (FunPtr)GetProcAddress(h, "startNetwork");
//		funPtr(&io, port);
//	}
	
	UnitTest::test();

	Logger::logInfo("$MyServer Start!!!");

	boost::asio::io_service::work work(io);
	io.run();

	finalizePython();
	Logger::logInfo("$MyServer exit!!!");

	return 0;
}