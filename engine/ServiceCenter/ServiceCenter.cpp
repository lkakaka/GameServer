#include "ServiceCenter.h"
#include "Logger.h"
#include "SCNet.h"
#include "SCMessageHandler.h"

using boost::asio::ip::tcp;


void initServiceCenter(boost::asio::io_service* io, int port) {
	new SCMessageHandler();
	SCNet* scNet = new SCNet(io);
	scNet->start(port);
}
