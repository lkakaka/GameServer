#include "SCConnection.h"
#include "Logger.h"
#include "SCMessageHandler.h"
#include "ServiceInfo.h"

#define MAX_MSG_LEN 64 * 1024

SCConnection::SCConnection(int connID, std::shared_ptr<tcp::socket> socket, ConnCloseCallback closeCallback) :
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
		LOG_ERROR("recv msg len error, %d", msgLen);
		setWaitClose("msg len error");
		return;
	}
	if (buffSize < msgLen + 16) return;
	
	int serviceGroup = m_recvBuffer.readInt();
	int serviceType = m_recvBuffer.readInt();
	int serviceId = m_recvBuffer.readInt();
	msgLen = m_recvBuffer.readInt();
	ServiceAddr dstAddr(serviceGroup, serviceType, serviceId);
	int errCode = SCMessageHandler::getSingleton()->onRecvConnectionMessage(this, &dstAddr, (char*)m_recvBuffer.data(), msgLen);
	if (errCode != MSG_ERROR_CODE::OK) {
		LOG_ERROR("sc conn msg error, errCode:%d", errCode);
		if (errCode == MSG_ERROR_CODE::VERIFY_FAIL) {
			setWaitClose("verify failed");
		}
		else {
			setWaitClose("msg error");
		}
		return;
	}
	m_recvBuffer.remove(msgLen);
	parse();
}

