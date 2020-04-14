
#include <iostream>
#include <thread>

#include "boost/asio.hpp"

#include "Network.h"
//#include "Log.h"
#include "Logger.h"
#include "ZmqInst.h"
#include "Timer.h"
#include "Config.h"
#include "MessageHandler.h"

using namespace std;

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

	std::string serviceName = Config::getConfigStr(cfgName, "service_name");
	if (serviceName.length() == 0) {
		printf("not config zmq name, file name: %s", cfgName);
		return 0;
	}
	Logger::initLog(serviceName.c_str());
	std::string routerAddr = Config::getConfigStr(cfgName, "router_addr");
	if (routerAddr.length() == 0) {
		Logger::logError("$not config router addr, file name: %s", cfgName);
		return 0;
	}

	boost::asio::io_service io;
	TimerMgr::initTimerMgr(&io);
	
	ZmqInst::initZmqInstance(serviceName.c_str(), routerAddr.c_str());
	ZmqInst::getZmqInstance()->setRecvCallback(MessageHandler::onRecvData);

	//Network::initNetwork(&io);
	int port = Config::getConfigInt(cfgName, "port");
	if (port <= 0) {
		Logger::logError("$not config port!!");
		return 0;
	}

	Network::initNetwork(&io, port);

	Logger::logInfo("$Gateway Start!!!");

	boost::asio::io_service::work work(io);
	io.run();

	Logger::logInfo("$Gateway exit!!!");

	return 0;
}