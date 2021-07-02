#include "SCConnection.h"
#include "Logger.h"
#include "SCMessageHandler.h"

#define MAX_MSG_LEN 64 * 1024

SCConnection::SCConnection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback) :
	ServerConnection(connID, socket, closeCallback),
	m_isVerify(false)
{

}

void SCConnection::parseMessage() {
	parse();
}

void SCConnection::parse() {
	int buffSize = m_recvBuffer.size();
	if (buffSize < 16) return;
	int msgLen = m_recvBuffer.getInt(12);
	if (msgLen < 0 || msgLen > MAX_MSG_LEN) {
		Logger::logError("$recv msg len error, %d", msgLen);
		close("msg len error");
		return;
	}
	if (buffSize < msgLen + 16) return;
	
	int serviceGroup = m_recvBuffer.readInt();
	int serviceType = m_recvBuffer.readInt();
	int serviceId = m_recvBuffer.readInt();
	msgLen = m_recvBuffer.readInt();
	ServiceAddr sender(serviceGroup, serviceType, serviceId);
	SCMessageHandler::getSingleton()->onRecvConnectionMessage(this, &sender, (char*)m_recvBuffer.data(), msgLen);
	m_recvBuffer.remove(msgLen);
	parse();
}

