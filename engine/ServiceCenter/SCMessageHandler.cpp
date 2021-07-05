#include "SCMessageHandler.h"
#include "Logger.h"
#include "Network/Network.h"
#include "SCNet.h"

extern const char* SERVICE_CONN_KEY;

INIT_SINGLETON_CLASS(SCMessageHandler)

void SCMessageHandler::onRecvMessage(ServiceAddr* sender, char* data, int dataLen) {
	
	
}

void SCMessageHandler::handleVerifyMsg(SCConnection* conn, ServiceAddr* sender, char* data, int len) {
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
	dispatchCacheMsg(conn);
}

void sendServiceMsg(SCConnection* dstConn, MyBuffer* buffer, ServiceAddr* srcAddr) {
	int size = buffer->size();
	dstConn->send((char*)buffer->data(), size);
	int msgId = -1;
	if (size >= 20) msgId = buffer->getInt(16);
	if (srcAddr != NULL) {
		Logger::logInfo("$dispatch msg %s->%s, msgId:%d, len:%d ", srcAddr->getName()->c_str(), dstConn->getServiceAddr()->getName()->c_str(), msgId, size - 20);
	}
	else {
		int serviceGroup = buffer->getInt(0);
		int serviceType = buffer->getInt(4);
		int serviceId = buffer->getInt(8);
		char addr[32]{ 0 };
		sprintf(addr, "%d.%d.%d", serviceGroup, serviceType, serviceId);
		Logger::logInfo("$dispatch msg %s->%s, msgId:%d, len:%d ", addr, dstConn->getServiceAddr()->getName()->c_str(), msgId, size - 20);
	}
	
}

void SCMessageHandler::dispatchServiceMsg(SCConnection* conn, ServiceAddr* dst, char* data, int len) {
	ServiceAddr* srcAddr = conn->getServiceAddr();
	MyBuffer buffer;
	buffer.writeInt(srcAddr->getServiceGroup());
	buffer.writeInt(srcAddr->getServiceType());
	buffer.writeInt(srcAddr->getServiceId());
	buffer.writeInt(len);
	buffer.writeString(data, len);
	
	SCConnection* dstConn = SCNet::getSingleton()->getServiceConnection(dst->getName()->c_str());
	if (dstConn != NULL) {
		sendServiceMsg(dstConn, &buffer, srcAddr);
		/*dstConn->send((char*)buffer.data(), buffer.size());
		int msgId = -1;
		if (len >= 4) msgId = buffer.getInt(16);
		Logger::logInfo("$dispatch msg %s->%s, msgId:%d, len:%d ", srcAddr->getName()->c_str(), dst->getName()->c_str(), msgId, len);*/
	}
	else {
		addMsgCache(dst, &buffer);
		Logger::logError("$dispatch msg error, dst:%s not connected!!", dst->getName()->c_str());
	}
}

void SCMessageHandler::addMsgCache(ServiceAddr* dst, MyBuffer* buffer) {
	std::string* dstAddr = dst->getName();
	auto pair = msgCaches.try_emplace(*dstAddr, std::vector<MyBuffer>());
	pair.first->second.push_back(*buffer);
}

void SCMessageHandler::dispatchCacheMsg(SCConnection* conn) {
	std::string* dstAddr = conn->getServiceAddr()->getName();
	auto iter = msgCaches.find(*dstAddr);
	if (iter == msgCaches.end()) return;
	std::vector<MyBuffer> msgs = iter->second;
	Logger::logInfo("$dispatch cache msg to %s, msg count:%d", dstAddr->c_str(), msgs.size());
	for (auto it = msgs.begin(); it != msgs.end(); it++) {
		sendServiceMsg(conn, &(*it), NULL);
	}
	msgCaches.erase(*dstAddr);
}

void SCMessageHandler::onRecvConnectionMessage(SCConnection* conn, ServiceAddr* addr, char* data, int dataLen) {
	if (!conn->isVerify()) {
		handleVerifyMsg(conn, addr, data, dataLen);
	} else {
		dispatchServiceMsg(conn, addr, data, dataLen);
	}

}
