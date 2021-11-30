#include "SCMessageHandler.h"
#include "Logger.h"
#include "Network/Network.h"
#include "SCNet.h"
#include "ServiceInfo.h"
#include "ServiceType.h"
#include "Network/ServiceCommEntityMgr.h"

extern const char* SERVICE_CONN_KEY;

#define SERVICE_ADDR_LEN 12

INIT_SINGLETON_CLASS(SCMessageHandler)

// 收到全局中心服发来的消息
void SCMessageHandler::onRecvMessage(ServiceAddr* dst, char* data, int dataLen) {
	if (dataLen < 16) {
		LOG_ERROR("SCMessageHandler recv message len error, dataLen: %d", dataLen);
		return;
	}
	SCConnection* dstConn = SCNet::getSingleton()->getServiceConnection(dst->getName());
	MyBuffer buffer(data, dataLen);
	int len = buffer.readInt();
	int srcServcieGroup = buffer.readInt();
	int srcServcieType = buffer.readInt();
	int srcServcieId = buffer.readInt();

	MyBuffer sendBuffer;
	sendBuffer.writeInt(srcServcieGroup);
	sendBuffer.writeInt(srcServcieType);
	sendBuffer.writeInt(srcServcieId);
	sendBuffer.writeInt(len - SERVICE_ADDR_LEN);
	sendBuffer.writeString((char*)buffer.data(), buffer.size());

	LOG_DEBUG("SCMessageHandler recv msg from group center, len:%d, dataLen:%d", len, dataLen);

	if (dstConn != NULL) {
		sendServiceMsg(dstConn, &sendBuffer, NULL);
	}
	else {
		addMsgCache(dst, &sendBuffer);
		LOG_ERROR("dispatch cross msg error, dst:%s not connected!!", dst->getName());
	}
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

void SCMessageHandler::sendServiceMsg(SCConnection* dstConn, MyBuffer* buffer, ServiceAddr* srcAddr) {
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

void SCMessageHandler::dispatchServiceMsg(SCConnection* conn, ServiceAddr* dstAddr, char* data, int len) {
	ServiceAddr* srcAddr = conn->getServiceAddr();
	MyBuffer buffer;
	// 不是服务器组内的消息，且当前不是全局中心服, 把消息往上一层发送
	if (dstAddr->getServiceGroup() != SERVICE_GROUP && SERVICE_TYPE != ServiceType::SERVICE_TYPE_GROUP_CENTER) {
		buffer.writeInt(len + SERVICE_ADDR_LEN);
		buffer.writeInt(srcAddr->getServiceGroup());
		buffer.writeInt(srcAddr->getServiceType());
		buffer.writeInt(srcAddr->getServiceId());
		buffer.writeString(data, len);
		LOG_DEBUG("dispatch msg to group center, len:%d", len + SERVICE_ADDR_LEN);
		GROUP_CENTER_COMM_ENTITY->sendToService(dstAddr, (char*) buffer.data(), buffer.size());
		return;
	}

	if (SERVICE_TYPE == ServiceType::SERVICE_TYPE_GROUP_CENTER) {
		buffer.writeInt(dstAddr->getServiceGroup());
		buffer.writeInt(dstAddr->getServiceType());
		buffer.writeInt(dstAddr->getServiceId());
		buffer.writeInt(len);
		buffer.writeString(data, len);
		LOG_DEBUG("dispatch msg to server center, len:%d", len);
	}
	else {
		buffer.writeInt(srcAddr->getServiceGroup());
		buffer.writeInt(srcAddr->getServiceType());
		buffer.writeInt(srcAddr->getServiceId());
		buffer.writeInt(len);
		buffer.writeString(data, len);
		LOG_DEBUG("dispatch msg to service, len:%d", len);
	}

	if (SERVICE_TYPE == ServiceType::SERVICE_TYPE_GROUP_CENTER) {
		dstAddr->changeServiceAddr(ServiceType::SERVICE_TYPE_CENTER, 0);
	}
	
	SCConnection* dstConn = SCNet::getSingleton()->getServiceConnection(dstAddr->getName());
	if (dstConn != NULL) {
		sendServiceMsg(dstConn, &buffer, srcAddr);
		/*dstConn->send((char*)buffer.data(), buffer.size());
		int msgId = -1;
		if (len >= 4) msgId = buffer.getInt(16);
		LOG_INFO("dispatch msg %s->%s, msgId:%d, len:%d ", srcAddr->getName(), dst->getName(), msgId, len);*/
	}
	else {
		addMsgCache(dstAddr, &buffer);
		LOG_ERROR("dispatch msg error, dst:%s not connected!!", dstAddr->getName());
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

int SCMessageHandler::onRecvConnectionMessage(SCConnection* conn, ServiceAddr* dstAddr, char* data, int dataLen) {
	if (!conn->isVerify()) {
		return handleVerifyMsg(conn, dstAddr, data, dataLen);
	} else {
		dispatchServiceMsg(conn, dstAddr, data, dataLen);
		return MSG_ERROR_CODE::OK;
	}

}
