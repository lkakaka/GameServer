#include "SCConnection.h"
#include "Logger.h"
#include "SCMessageHandler.h"

SCConnection::SCConnection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback) :
	Connection(connID, socket, closeCallback),
	m_isVerify(false)
{

}

void SCConnection::onRead(char* data, int len) {
	Logger::logDebug("$read data, len=%d", len);
	m_recvBuffer.writeString(data, len);
	int buffSize = m_recvBuffer.size();
	if (buffSize < 16) return;
	int msgLen = m_recvBuffer.getInt(12);
	if (buffSize < msgLen + 16) return;
	
	int serviceGroup = m_recvBuffer.readInt();
	int serviceType = m_recvBuffer.readInt();
	int serviceId = m_recvBuffer.readInt();
	msgLen = m_recvBuffer.readInt();
	ServiceAddr sender(serviceGroup, serviceType, serviceId);
	SCMessageHandler::getSingleton()->onRecvConnectionMessage(this, &sender, (char*)m_recvBuffer.data(), msgLen);
	/*if (!m_isVerify) {
		handleVerifyMsg(m_recvBuffer.data(), msgLen);
	}
	else {
		handleServiceMsg(m_recvBuffer.data(), msgLen);
	}*/
	m_recvBuffer.remove(msgLen);
}

