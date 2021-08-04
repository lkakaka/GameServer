
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
#include "Logger.h"
#include "DBMgr.h"
//#include "py/PythonPlugin.h"
//#include "../Common/PyCommon.h"
#include "../Common/ServerMacros.h"
#include "Timer.h"
#include "UnitTest.h"
#include "Config.h"
#include "MessageHandler.h"
#include "GameService.h"
#include "server.hpp"
#include "CmdLine.h"
#include "ServiceInfo.h"

//#include "lua/LuaPlugin.h"
#include "GatewayEntry.h"
#include "ServiceCenter.h"
#include "Network/ServiceCommEntityMgr.h"

using namespace std;

std::string getServerConfigStr(const char* key);
static int getServerConfigInt(const char* key);
static void initServiceCommEntity(boost::asio::io_service* io);
	
static boost::asio::io_service io;


static void signalHandler( int signum )
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";
	Logger::logInfo("$stop server!!!!");
	io.stop();
 
    // 靠靠?
    //     // 靠靠 
          
    //exit(signum);  
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("arg count error");
		return 0;
	}
	const char* cfgName = argv[1];
	if (!Config::checkFileExist(cfgName)) {
		printf("cfg file not exist, file name: %s", cfgName);
		return 0;
	}
	Config* config = new Config(cfgName);

	int serverId = getServerConfigInt("server_id");
	if (serverId <= 0) {
		printf("$not config server id, config file: %s", cfgName);
		return 0;
	}

	std::string serviceName = getServerConfigStr("service_name");
	if (serviceName.length() == 0) {
		printf("$not config service name, file name: %s", cfgName);
		return 0;
	}

	ServiceType serviceType = getServiceType(serviceName);
	if (serviceType <= ServiceType::SERVICE_TYPE_START || serviceType >= ServiceType::SERVICE_TYPE_END) {
		printf("$service type error, serviceName: %s", serviceName.c_str());
		return 0;
	}

	int serviceId = getServerConfigInt("service_id");
	if (serviceId < 0) {
		printf("$service id error, serviceName: %s", serviceName.c_str());
		return 0;
	}
	
	//boost::asio::io_service io;
	signal(SIGTERM, signalHandler);

	ServiceInfo* serviceInfo = new ServiceInfo(serverId, serviceType, serviceId);

	std::string logFileName = serviceName;
	if (serviceId > 0) logFileName += "_" + std::to_string(serviceId);
	Logger::initLog(logFileName.c_str());
	Logger::logInfo("$dfas,%%n");

	std::string dbUrl = getServerConfigStr("db_url");
	if (dbUrl.length() > 0) {
		std::string dbUserName = getServerConfigStr("db_username");
		std::string dbPassword = getServerConfigStr("db_password");
		int dbPort = getServerConfigInt("db_port");
		new DBMgr(dbUserName, dbPassword, dbUrl, dbPort);
		Logger::logInfo("$db config, url: %s, port:%d", dbUrl.c_str(), dbPort);
	}

	TimerMgr::initTimerMgr(&io);
	
	//PyObject* scriptObj = NULL;
	std::string funcName = getServerConfigStr("script_init_func");
	//if (funcName.length() > 0) {
	//	/*initPython();
	//	auto py_state = PyGILState_Ensure();
	//	scriptObj = callPyFunction("main", funcName.c_str(), NULL);
	//	PyGILState_Release(py_state);

	//	new LuaPlugin();
	//	LuaPlugin::getLuaPlugin()->initLua(funcName.c_str());*/
	//}

	GameService::g_gameService = new GameService(serviceName, serviceType);
	GameService::g_gameService->initScript(funcName.c_str());

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


	if (serviceType == SERVICE_TYPE_CENTER) {
		initServiceCenter(&io);
	}
	else {
		initServiceCommEntity(&io);
	}

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

	Logger::logInfo("$MyServer exit!!!");

	return 0;
}


std::string getServerConfigStr(const char* key) {
	return Config::getSingleton()->getConfigStr(key);
}

int getServerConfigInt(const char* key) {
	return Config::getSingleton()->getConfigInt(key);
}

void initServiceCommEntity(boost::asio::io_service* io) {
	std::string centerServiceIp = getServerConfigStr("center_service_ip");
	if (centerServiceIp.length() == 0) {
		Logger::logError("$not config center service ip, file name: %s", Config::getSingleton()->getConfigFileName());
		THROW_EXCEPTION("not config center service ip");
	}

	int centerServicePort = getServerConfigInt("center_service_port");
	if (centerServicePort <= 0) {
		Logger::logError("$not config center service port, file name: %s", Config::getSingleton()->getConfigFileName());
		THROW_EXCEPTION("not config center service port");
	}

	//ServiceAddr* addr = GameService::g_gameService->getServiceAddr();
	int serviceGroup = ServiceInfo::getSingleton()->getServiceGroup();
	int serviceType = ServiceInfo::getSingleton()->getServiceType();
	int serviceId = ServiceInfo::getSingleton()->getServiceId();
	ServiceAddr addr(serviceGroup, serviceType, serviceId);

	CommEntityMgr* commEntityMgr = new CommEntityMgr();
	IServiceCommEntity* commEntity = commEntityMgr->createCommEntity(io, addr, centerServiceIp.c_str(), centerServicePort);

	if (serviceType == SERVICE_TYPE_GATEWAY) {
		int port = getServerConfigInt("port");
		initGateway(io, port);
	}
	else {
		ServiceMessageHandler* messageHandler = new ServiceMessageHandler();
		commEntity->setMessageHandler(messageHandler);
		//commEntity->setRecvCallback(std::bind(&ServiceMessageHandler::onRecvMessage, messageHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	commEntity->start();
}
