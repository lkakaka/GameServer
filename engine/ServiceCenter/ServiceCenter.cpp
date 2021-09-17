#include "ServiceCenter.h"
#include "Logger.h"
#include "SCNet.h"
#include "SCMessageHandler.h"
#include "Config.h"
#include "../Common/ServerMacros.h"
#include "Network/Network.h"

#ifdef SERVICE_COMM_ENTITY_ZMQ
#include "Network/ZmqRouter.h"
#endif

using boost::asio::ip::tcp;


void initServiceCenter(boost::asio::io_service* io) {
	int port = Config::getSingleton()->getConfigInt("port");
	if (port <= 0) {
		LOG_ERROR("not config port!!!");
		THROW_EXCEPTION("not config port");
	}
	new SCMessageHandler();

	#ifdef SERVICE_COMM_ENTITY_ZMQ
		std::string serviceName = Config::getSingleton()->getConfigStr("service_name");
		ZmqRouter::initZmqRouter(serviceName.c_str(), port);
	#else
		SCNet* scNet = new SCNet(io);
		scNet->start(port);
	#endif
}
