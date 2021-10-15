
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
#include "Logger.h"
#include "DBMgr.h"
#include "../Common/ServerMacros.h"
#include "Timer.h"
#include "UnitTest.h"
#include "Config.h"
#include "MessageHandler.h"
#include "GameService.h"
#include "server.hpp"
#include "CmdLine.h"
#include "ServiceInfo.h"
#include "ExitTraceback.h"

#include "GatewayEntry.h"
#include "ServiceCenter.h"
#include "Network/ServiceCommEntityMgr.h"
#include "http_client/HttpClientMgr.h"

using namespace std;

static void initServiceCommEntity(boost::asio::io_service* io);
	
static boost::asio::io_service io;


static void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";
	LOG_INFO("stop server!!!!");
	io.stop();
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

	int serverId = GET_CONFG_INT("server_id");
	if (serverId <= 0) {
		printf("$not config server id, config file: %s", cfgName);
		return 0;
	}

	std::string serviceName = GET_CONFG_STR("service_name");
	if (serviceName.length() == 0) {
		printf("$not config service name, file name: %s", cfgName);
		return 0;
	}

	ServiceType serviceType = getServiceType(serviceName);
	if (serviceType <= ServiceType::SERVICE_TYPE_START || serviceType >= ServiceType::SERVICE_TYPE_END) {
		printf("$service type error, serviceName: %s", serviceName.c_str());
		return 0;
	}

	int serviceId = GET_CONFG_INT("service_id");
	if (serviceId < 0) {
		printf("$service id error, serviceName: %s", serviceName.c_str());
		return 0;
	}
	
	signal(SIGTERM, signalHandler);
	//signal(SIGSEGV, signalExit);
	signal(SIGABRT, signalExit);

	ServiceInfo* serviceInfo = new ServiceInfo(serverId, serviceType, serviceId);

	std::string logFileName = serviceName;
	if (serviceId > 0) logFileName += "_" + std::to_string(serviceId);
	Logger::initLog(logFileName.c_str());
	LOG_INFO("dfas,%%n");

	std::string dbUrl = GET_CONFG_STR("db_url");
	if (dbUrl.length() > 0) {
		std::string dbUserName = GET_CONFG_STR("db_username");
		std::string dbPassword = GET_CONFG_STR("db_password");
		int dbPort = GET_CONFG_INT("db_port");
		new DBMgr(dbUserName, dbPassword, dbUrl, dbPort);
		LOG_INFO("db config, url: %s, port:%d", dbUrl.c_str(), dbPort);
	}

	TimerMgr::initTimerMgr(&io);
	HttpClientMgr::init(&io);
	
	std::string funcName = GET_CONFG_STR("script_init_func");
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
	//	LOG_INFO("start http server, ip: %s, port:%s", httpServerIp.c_str(), httpServerPort.c_str());
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
//			LOG_ERROR("gateway.dll not found");
//			return 1;
//		}
//		typedef void(*FunPtr)(boost::asio::io_service*, int);//定义函数指针
//		FunPtr funPtr = (FunPtr)GetProcAddress(h, "startNetwork");
//		funPtr(&io, port);
//	}
	
	UnitTest::test();

	startCmd();

	LOG_INFO("MyServer Start!!!");

	boost::asio::io_service::work work(io);
	io.run();

	LOG_INFO("MyServer exit!!!");

	return 0;
}

void initServiceCommEntity(boost::asio::io_service* io) {
	std::string centerServiceIp = GET_CONFG_STR("center_service_ip");
	if (centerServiceIp.length() == 0) {
		LOG_ERROR("not config center service ip, file name: %s", Config::getSingleton()->getConfigFileName());
		THROW_EXCEPTION("not config center service ip");
	}

	int centerServicePort = GET_CONFG_INT("center_service_port");
	if (centerServicePort <= 0) {
		LOG_ERROR("not config center service port, file name: %s", Config::getSingleton()->getConfigFileName());
		THROW_EXCEPTION("not config center service port");
	}

	ServiceAddr commAddr(SERVICE_GROUP, SERVICE_TYPE, SERVICE_ID);
	CommEntityMgr* commEntityMgr = new CommEntityMgr();
	IServiceCommEntity* commEntity = commEntityMgr->createCommEntity(io, commAddr, centerServiceIp.c_str(), centerServicePort);

	if (SERVICE_TYPE == SERVICE_TYPE_GATEWAY) {
		initGateway(io);
	} else {
		ServiceMessageHandler* messageHandler = new ServiceMessageHandler();
		commEntity->setMessageHandler(messageHandler);
	}
	commEntity->start();
}
