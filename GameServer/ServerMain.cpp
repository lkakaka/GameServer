
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
//#include "DBPlugin.h"
#include "Logger.h"
//#include "DBTableDef.h"
#include "DBMgr.h"
#include "PythonPlugin.h"
#include "../Common/PyCommon.h"
#include "ZmqInst.h"
#include "ZmqRouter.h"
#include "Timer.h"
#include "UnitTest.h"
#include "Config.h"
#include "MessageMgr.h"
#include "GameService.h"
#include "server.hpp"
#include "Network.h"
#include "CmdLine.h"

#include "mysqlx/xdevapi.h"

using namespace std;
//GameService g_game_service;
//extern struct GameService g_game_service;
std::string g_cfgFileName = "";


int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("arg count error");
		return 0;
	}
	char* cfgName = argv[1];
	if (!Config::checkFileExist(cfgName)) {
		printf("cfg file not exist, file name: %s", cfgName);
		return 0;
	}

	g_cfgFileName = cfgName;

	std::string serviceName = Config::getConfigStr(cfgName, "service_name");
	if (serviceName.length() == 0) {
		printf("$not config service name, file name: %s", cfgName);
		return 0;
	}

	ServiceType serviceType = getServiceType(serviceName);
	if (serviceType <= ServiceType::SERVICE_TYPE_START || serviceType >= ServiceType::SERVICE_TYPE_END) {
		printf("$service type error, serviceName: %s", serviceName.c_str());
		return 0;
	}

	Logger::initLog(serviceName.c_str());
	Logger::logInfo("$dfas,%%n");
	//g_game_service.service_name = serviceName;

	std::string dbUrl = Config::getConfigStr(cfgName, "db_url");
	if (dbUrl.length() > 0) {
		DBMgr::m_dbUrl = dbUrl;
		DBMgr::m_dbUserName = Config::getConfigStr(cfgName, "db_username");;
		DBMgr::m_dbPassword = Config::getConfigStr(cfgName, "db_password");;
		DBMgr::m_dbPort = Config::getConfigInt(cfgName, "db_port");
		Logger::logInfo("$db config, url: %s, port:%d", dbUrl.c_str(), DBMgr::m_dbPort);
	}

	boost::asio::io_service io;
	TimerMgr::initTimerMgr(&io);

	/*ZmqInst::initZmqInstance(serviceName.c_str(), routerAddr.c_str());
	ZmqInst::getZmqInstance()->setRecvCallback(MessageMgr::onRecvData);*/
	
	PyObject* scriptObj = NULL;
	std::string funcName = Config::getConfigStr(cfgName, "script_init_func");
	if (funcName.length() > 0) {
		initPython();
		auto py_state = PyGILState_Ensure();
		scriptObj = callPyFunction("main", funcName.c_str(), NULL);
		PyGILState_Release(py_state);
	}

	GameService::g_gameService = new GameService(serviceName, serviceType, scriptObj);

	// Initialise the http server.
	//std::string httpServerIp = Config::getConfigStr(cfgName, "http_server_ip");
	//std::string httpServerPort = Config::getConfigStr(cfgName, "http_server_port");
	//std::shared_ptr<std::thread> http_thread;
	//std::shared_ptr<http::server::server> http_server;
	//if (httpServerIp.length() > 0 && httpServerPort.length() > 0) {
	//	http_server.reset(new http::server::server(httpServerIp, httpServerPort, "", NULL));

	//	// Run the server until stopped.
	//	http_thread.reset(new std::thread([&http_server] { http_server->run(); }));
	//	Logger::logInfo("$start http server, ip: %s, port:%s", httpServerIp.c_str(), httpServerPort.c_str());
	//}

	int router_port = Config::getConfigInt(cfgName, "router_port");
	if (router_port > 0) {
		ZmqRouter::initZmqRouter(serviceName.c_str(), router_port);
	}
	else {
		std::string routerAddr = Config::getConfigStr(cfgName, "router_addr");
		if (routerAddr.length() == 0) {
			Logger::logError("$not config router addr, file name: %s", cfgName);
			return 0;
		}

		ZmqInst::initZmqInstance(serviceName.c_str(), routerAddr.c_str());
		int port = Config::getConfigInt(cfgName, "port");
		if (port > 0) {
			Logger::logInfo("$gateway port:%d", port);
			Network::initNetwork(&io, port);
			ZmqInst::getZmqInstance()->setRecvCallback(MessageMgr::onGatewayRecvData);
		}
		else {
			ZmqInst::getZmqInstance()->setRecvCallback(MessageMgr::onRecvData);
		}
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

	startCmd();

	Logger::logInfo("$MyServer Start!!!");

	boost::asio::io_service::work work(io);
	io.run();

	finalizePython();
	Logger::logInfo("$MyServer exit!!!");

	return 0;
}
