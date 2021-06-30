#include "GatewayEntry.h"
#include "Logger.h"
#include "Network.h"
#include "GatewayMessageHandler.h"
#include "ServiceCommEntityMgr.h"


void initGateway(boost::asio::io_service* io, int port) {
	Logger::logInfo("$gateway port:%d", port);
	Network::initNetwork(io, port);
	GatewayMessageHandler* messageHandler = new GatewayMessageHandler();
	CommEntityMgr::getSingleton()->getCommEntity()->setRecvCallback(std::bind(&GatewayMessageHandler::onRecvMessage, messageHandler, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}