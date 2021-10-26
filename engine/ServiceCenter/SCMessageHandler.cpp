#include "SCMessageHandler.h"
#include "Logger.h"
#include "Network/Network.h"
#include "SCNet.h"

extern const char* SERVICE_CONN_KEY;

INIT_SINGLETON_CLASS(SCMessageHandler)

void SCMessageHandler::onRecvMessage(ServiceAddr* sender, char* data, int dataLen) {
	
	
}

int SCMessageHandler::handleVerifyMsg(SCConnection* conn, ServiceAddr* sender, char* data, int len) {
	int keyLen = strlen(SERVICE_CONN_KEY);
	if (keyLen != len) {
		LOG_ERROR("verify msg len mismatch! len:%d,%d", len, keyLen);
		return MSG_ERROR_CODE::VERIFY_FAIL;
	}
	if (std::strncmp(SERVICE_CONN_KEY, (char*)data, len) != 0) {
		LOG_ERROR("verify msg content mismatch!");
		return MSG_ERROR_CODE::VERIFY_FAIL;
	}
	conn->setVerify(true);
	conn->setServiceAddr(*sender);
	SCNet::getSingleton()->addServiceConnection(sender->getName(), conn);
	LOG_INFO("service %s connected!!, connId:%d", sender->getName(), conn->getConnID());
	dispatchCacheMsg(conn);
	return MSG_ERROR_CODE::OK;
}

void sendServiceMsg(SCConnection* dstConn, MyBuffer* buffer, ServiceAddr* srcAddr) {
	int size = buffer->size();
	//dstConn->send((char*)buffer->data(), size);
	dstConn->send(std::move(buffer->getBuf()));
	int msgId = -1;
	if (size >= 20) msgId = buffer->getInt(16);
	if (srcAddr != NULL) {
		LOG_INFO("dispatch msg %s->%s, msgId:%d, len:%d ", srcAddr->getName(), dstConn->getServiceAddr()->getName(), msgId, size - 20);
	}
	else {
		int serviceGroup = buffer->getInt(0);
		int serviceType = buffer->getInt(4);
		int serviceId = buffer->getInt(8);
		char addr[32]{ 0 };
		sprintf(addr, "%d.%d.%d", serviceGroup, serviceType, serviceId);
		LOG_INFO("dispatch msg %s->%s, msgId:%d, len:%d ", addr, dstConn->getServiceAddr()->getName(), msgId, size - 20);
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
	
	SCConnection* dstConn = SCNet::getSingleton()->getServiceConnection(dst->getName());
	if (dstConn != NULL) {
		sendServiceMsg(dstConn, &buffer, srcAddr);
		/*dstConn->send((char*)buffer.data(), buffer.size());
		int msgId = -1;
		if (len >= 4) msgId = buffer.getInt(16);
		LOG_INFO("dispatch msg %s->%s, msgId:%d, len:%d ", srcAddr->getName(), dst->getName(), msgId, len);*/
	}
	else {
		addMsgCache(dst, &buffer);
		LOG_ERROR("dispatch msg error, dst:%s not connected!!", dst->getName());
	}
}

void SCMessageHandler::addMsgCache(ServiceAddr* dst, MyBuffer* buffer) {
	const char* dstAddr = dst->getName();
	/*auto iter = msgCaches.find(dstAddr);
	if (iter == msgCaches.end()) {
		msgCaches.emplace(dstAddr, std::vector<MyBuffer>());
		iter = msgCaches.find(dstAddr);
	}
	iter->second.push_back(*buffer);*/
	auto pair = msgCaches.try_emplace(dstAddr, std::vector<MyBuffer>());
	pair.first->second.push_back(*buffer);
}

void SCMessageHandler::dispatchCacheMsg(SCConnection* conn) {
	const char* dstAddr = conn->getServiceAddr()->getName();
	auto iter = msgCaches.find(dstAddr);
	if (iter == msgCaches.end()) return;
	std::vector<MyBuffer> msgs = iter->second;
	LOG_INFO("dispatch cache msg to %s, msg count:%d", dstAddr, msgs.size());
	for (auto it = msgs.begin(); it != msgs.end(); it++) {
		sendServiceMsg(conn, &(*it), NULL);
	}
	msgCaches.erase(dstAddr);
}

int SCMessageHandler::onRecvConnectionMessage(SCConnection* conn, ServiceAddr* addr, char* data, int dataLen) {
	if (!conn->isVerify()) {
		return handleVerifyMsg(conn, addr, data, dataLen);
	} else {
		dispatchServiceMsg(conn, addr, data, dataLen);
		return MSG_ERROR_CODE::OK;
	}

}
