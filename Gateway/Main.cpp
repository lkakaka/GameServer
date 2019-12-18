
#include <iostream>
#include <thread>

#include "boost/asio.hpp"

#include "Network.h"
//#include "Log.h"
#include "Logger.h"
#include "ZmqInst.h"
#include "Timer.h"
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

	boost::asio::io_service io;
	TimerMgr::initTimerMgr(&io);

	std::string zmqAddr = Config::getConfigStr(cfgName, "zmq_addr");
	ZmqInst::initZmqInstance(zmqAddr.c_str());
	ZmqInst::getZmqInstance()->setRecvCallback(ProtoBufferMgr::onRecvData);

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