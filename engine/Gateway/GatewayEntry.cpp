#include "GatewayEntry.h"
#include "Logger.h"
#include "GatewayNet.h"
#include "GatewayMessageHandler.h"
#include "Network/ServiceCommEntityMgr.h"


void initGateway(boost::asio::io_service* io, int port) {
	Logger::logInfo("$gateway port:%d", port);
	GatewayNet* gatewayNet = new GatewayNet();
	gatewayNet->start(*io, port);
	GatewayMessageHandler* messageHandler = new GatewayMessageHandler();
	CommEntityMgr::getSingleton()->getCommEntity()->setRecvCallback(std::bind(&GatewayMessageHandler::onRecvMessage, messageHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}