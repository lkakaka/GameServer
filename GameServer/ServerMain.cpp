
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
#include "MessageMgr.h"
#include "GameService.h"
#include "server.hpp"

using namespace std;
std::string g_service_name = "";
//GameService g_game_service;
//extern struct GameService g_game_service;

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

	std::string serviceName = Config::getConfigStr(cfgName, "service_name");
	if (serviceName.length() == 0) {
		Logger::logError("$not config zmq name, file name: %s", cfgName);
		return 0;
	}
	//g_game_service.service_name = serviceName;

	std::string routerAddr = Config::getConfigStr(cfgName, "router_addr");
	if (routerAddr.length() == 0) {
		Logger::logError("$not config router addr, file name: %s", cfgName);
		return 0;
	}

	boost::asio::io_service io;
	TimerMgr::initTimerMgr(&io);
	
	PyObject* scriptObj = NULL;
	std::string funcName = Config::getConfigStr(cfgName, "script_init_func");
	if (funcName.length() > 0) {
		initPython();
		auto py_state = PyGILState_Ensure();
		scriptObj = callPyFunction("main", funcName.c_str(), NULL);
		PyGILState_Release(py_state);
	}

	GameService::g_gameService = new GameService(serviceName, scriptObj);

	ZmqInst::initZmqInstance(serviceName.c_str(), routerAddr.c_str());
	ZmqInst::getZmqInstance()->setRecvCallback(MessageMgr::onRecvData);

	// Initialise the http server.
	std::string httpServerIp = Config::getConfigStr(cfgName, "http_server_ip");
	std::string httpServerPort = Config::getConfigStr(cfgName, "http_server_port");
	std::shared_ptr<std::thread> http_thread;
	std::shared_ptr<http::server::server> http_server;
	if (httpServerIp.length() > 0 && httpServerPort.length() > 0) {
		http_server.reset(new http::server::server(httpServerIp, httpServerPort, ""));

		// Run the server until stopped.
		http_thread.reset(new std::thread([&http_server] { http_server->run(); }));
	}

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