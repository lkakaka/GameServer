#include "ServiceCenter.h"
#include "Logger.h"
#include "SCNet.h"
#include "SCMessageHandler.h"
#include "Config.h"
#include "../Common/ServerMacros.h"
#include "Network/Network.h"
#include "GameService.h" 

#ifdef SERVICE_COMM_ENTITY_ZMQ
#include "Network/ZmqRouter.h"
#endif

#include "Network/ServiceCommEntityMgr.h"

using boost::asio::ip::tcp;


void initServiceCenter(boost::asio::io_service* io) {
	int port = Config::getSingleton()->getConfigInt("port");
	if (port <= 0) {
		LOG_ERROR("not config port!!!");
		THROW_EXCEPTION("not config port");
	}
	SCMessageHandler* messageHandler = new SCMessageHandler();

	#ifdef SERVICE_COMM_ENTITY_ZMQ
		std::string serviceName = Config::getSingleton()->getConfigStr("service_name");
		ZmqRouter::initZmqRouter(serviceName.c_str(), port);
	#else
		SCNet* scNet = new SCNet(io);
		scNet->start(port);
	#endif

	std::string group_center_ip = Config::getSingleton()->getConfigStr("group_center_ip");
	int group_center_port = Config::getSingleton()->getConfigInt("group_center_port");
	if (group_center_ip.length() > 0 && group_center_port > 0) {
		ServiceAddr commAddr(SERVICE_GROUP, SERVICE_TYPE, SERVICE_ID);
		CommEntityMgr* commEntityMgr = new CommEntityMgr();
		IServiceCommEntity* commEntity = commEntityMgr->createCommEntity(GROUP_CENTER_COMM_NAME, commAddr, group_center_ip.c_str(), group_center_port);
		commEntity->setMessageHandler(messageHandler);
		commEntity->start(); 
	}
}
