#include "SCMessageHandler.h"
#include "Logger.h"
#include "Network/Network.h"
#include "SCNet.h"

extern const char* SERVICE_CONN_KEY;

INIT_SINGLETON_CLASS(SCMessageHandler)

void SCMessageHandler::onRecvMessage(ServiceAddr* sender, char* data, int dataLen) {
	
	
}

void handleVerifyMsg(SCConnection* conn, ServiceAddr* sender, char* data, int len) {
	int keyLen = strlen(SERVICE_CONN_KEY);
	if (keyLen != len) {
		Logger::logError("$verify msg len mismatch! len:%d,%d", len, keyLen);
		SCNet::getSingleton()->closeConnection(conn, "verify failed");
		return;
	}
	if (std::strncmp(SERVICE_CONN_KEY, (char*)data, len) != 0) {
		Logger::logError("$verify msg content mismatch!");
		SCNet::getSingleton()->closeConnection(conn, "verify failed");
		return;
	}
	conn->setVerify(true);
	conn->setServiceAddr(*sender);
	SCNet::getSingleton()->addServiceConnection(sender->getName()->c_str(), conn);
	Logger::logInfo("$service %s connected!!, connId:%d", sender->getName()->c_str(), conn->getConnID());
}

void dispatchServiceMsg(SCConnection* conn, ServiceAddr* dst, char* data, int len) {
	SCConnection* dstConn = SCNet::getSingleton()->getServiceConnection(dst->getName()->c_str());
	if (dstConn != NULL) {
		ServiceAddr* srcAddr = conn->getServiceAddr();
		MyBuffer buffer;
		buffer.writeInt(srcAddr->getServiceGroup());
		buffer.writeInt(srcAddr->getServiceType());
		buffer.writeInt(srcAddr->getServiceId());
		buffer.writeInt(len);
		buffer.writeString(data, len);
		dstConn->send((char*)buffer.data(), buffer.size());
		int msgId = -1;
		if (len >= 4) msgId = buffer.getInt(16);
		Logger::logInfo("$dispatch msg %s->%s, msgId:%d, len:%d ", srcAddr->getName()->c_str(), dst->getName()->c_str(), msgId, len);
	}
	else {
		// todo: ´æ´¢ÏûÏ¢
		Logger::logError("$dispatch msg error, dst:%s not connected!!", dst->getName()->c_str());
	}
}

void SCMessageHandler::onRecvConnectionMessage(SCConnection* conn, ServiceAddr* addr, char* data, int dataLen) {
	if (!conn->isVerify()) {
		handleVerifyMsg(conn, addr, data, dataLen);
	} else {
		dispatchServiceMsg(conn, addr, data, dataLen);
	}

}
