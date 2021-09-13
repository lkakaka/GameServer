#include "GatewayEntry.h"
#include "Logger.h"
#include "GatewayNet.h"
#include "GatewayMessageHandler.h"
#include "Network/ServiceCommEntityMgr.h"
#include "Config.h"
#include "../Common/ServerMacros.h"


void initGateway(boost::asio::io_service* io) {
	int port = GET_CONFG_INT("port");
	int udpPort = GET_CONFG_INT("udp_port");
	Logger::logInfo("$gateway port:%d, updPort:%d", port, udpPort);
	if (port <= 0) {
		THROW_EXCEPTION("not config port!!!");
	}
	GatewayNet* gatewayNet = new GatewayNet(io);
	gatewayNet->start(port);
	if (udpPort > 0) gatewayNet->startUdp(udpPort);
	GatewayMessageHandler* messageHandler = new GatewayMessageHandler();
	CommEntityMgr::getSingleton()->getCommEntity()->setMessageHandler(messageHandler);
	//CommEntityMgr::getSingleton()->getCommEntity()->setRecvCallback(std::bind(&GatewayMessageHandler::onRecvMessage, messageHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}