
#include <iostream>
#include <thread>

#include "boost/asio.hpp"
#include "AsioService.h"
#include "TaskMgr.h"
#include "Logger.h"
#include "DBMgr.h"
#include "../Common/ServerMacros.h"
#include "Timer.h"
#include "UnitTest.h"
#include "Config.h"
#include "MessageHandler.h"
#include "GameService.h"
#include "ScriptBind.h"
#include "server.hpp"
#include "CmdLine.h"
#include "ExitTraceback.h"

#include "GatewayEntry.h"
#include "ServiceCenter.h"
#include "Network/ServiceCommEntityMgr.h"
#include "http_client/HttpClientMgr.h"

using namespace std;

static void initServiceCommEntity(boost::asio::io_service* io);

static std::thread* g_sinal_thread = NULL;
static bool _stop = false;

static void stopServer()
{
    //std::cout << "Interrupt signal (" << signum << ") received.\n";
	//thread::id id = std::this_thread::get_id();
	LOG_INFO("stop server!!!!");
	_stop = true;
	MAIN_IO_SERVICE_PTR->async_run_task([]() { AsioServiceMgr::getSingleton()->stopAll(); });
}

static void signalThreadFunc() {
#ifndef WIN32
	//sigset_t waitset;
	//int signum;
	//sigemptyset(&waitset);
	//sigaddset(&waitset, SIGTERM);
	//struct timespec timeout = { 1, 0 };
	//while (!_stop) {
	//	if (-1 == (signum = sigtimedwait(&waitset, NULL, &timeout))) {
	//		//do not log error, because timeout will also return -1.
	//		printf("time out or error, errno=%d, errmsg=%s\n", errno, strerror(errno));
	//	}
	//	else {
	//		printf("sigwaitinfo() fetch the signal: %d\n", signum);
	//		stopServer();
	//	}
	//}

	sigset_t waitset;
	sigemptyset(&waitset);
	sigaddset(&waitset, SIGTERM);
	int res, sig;
	while(!_stop) {
		res = sigwait(&waitset, &sig);
		if (res == 0) {
			if (sig == SIGTERM) {
				LOG_INFO("recv SIGTERM!!!");
				stopServer();
			}
		} else {
			LOG_ERROR("sigwait error, %d", res);
		}
	}

#endif // WIN32
}

static void startSignalHandlerThread() {
#ifndef WIN32
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);
	pthread_sigmask(SIG_SETMASK, &mask, NULL);//SIG_BLOCK SIG_SETMASK 会屏蔽掉SIGTERM，但SIG_UNBLOCK不会屏蔽SIGTERM
	g_sinal_thread = new std::thread(signalThreadFunc);
	LOG_INFO("start signal thread!!!!");
#endif // WIN32
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
		printf("not config server id, config file: %s", cfgName);
		return 0;
	}

	std::string serviceName = GET_CONFG_STR("service_name");
	if (serviceName.length() == 0) {
		printf("not config service name, file name: %s", cfgName);
		return 0;
	}

	ServiceType serviceType = getServiceType(serviceName);
	if (serviceType < ServiceType::SERVICE_TYPE_START || serviceType >= ServiceType::SERVICE_TYPE_END) {
		printf("service type error, serviceName: %s", serviceName.c_str());
		return 0;
	}

	int serviceId = GET_CONFG_INT("service_id");
	if (serviceId < 0) {
		printf("service id error, serviceName: %s", serviceName.c_str());
		return 0;
	}

	AsioServiceMgr* asioServiceMgr = new AsioServiceMgr();
	AsioService* ioService = asioServiceMgr->createAsioService();
	boost::asio::io_service* io = ioService->getIoService();
	
	//signal(SIGSEGV, signalExit);
	signal(SIGABRT, signalExit);

	std::string logFileName = serviceName;
	if (serviceId > 0) logFileName += "_" + std::to_string(serviceId);
	Logger::initLog(serverId, logFileName.c_str());
	LOG_INFO("dfas,%%n");

	startSignalHandlerThread();

	std::string dbUrl = GET_CONFG_STR("db_url");
	if (dbUrl.length() > 0) {
		std::string dbUserName = GET_CONFG_STR("db_username");
		std::string dbPassword = GET_CONFG_STR("db_password");
		int dbPort = GET_CONFG_INT("db_port");
		new DBMgr(dbUserName, dbPassword, dbUrl, dbPort);
		LOG_INFO("db config, url: %s, port:%d", dbUrl.c_str(), dbPort);
	}

	TimerMgr::initTimerMgr();
	HttpClientMgr::init();
	
	std::string funcName = GET_CONFG_STR("script_init_func");
	IScript* script = ScriptBind::bindScript(funcName.c_str());
	GameService* service = new GameService(serverId, serviceType, serviceId, script);
	//GameService::g_gameService->initScript(funcName.c_str());

	if (serviceType == SERVICE_TYPE_CENTER || serviceType == SERVICE_TYPE_GROUP_CENTER) {
		initServiceCenter(io);
	}
	else {
		initServiceCommEntity(io);
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

	/*boost::asio::io_service::work work(*io);
	io->run();*/
	MAIN_IO_SERVICE_PTR->run();

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
	IServiceCommEntity* commEntity = commEntityMgr->createCommEntity(SERVER_CENTER_COMM_NAME, commAddr, centerServiceIp.c_str(), centerServicePort);

	if (SERVICE_TYPE == SERVICE_TYPE_GATEWAY) {
		initGateway(io);
	} else {
		ServiceMessageHandler* messageHandler = new ServiceMessageHandler();
		commEntity->setMessageHandler(messageHandler);
	}
	commEntity->start();
}
