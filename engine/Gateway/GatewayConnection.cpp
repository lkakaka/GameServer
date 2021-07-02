#include "GatewayConnection.h"
#include "proto.h"
#include "Network/ServiceCommEntityMgr.h"
#include "ServiceType.h"
#include "ServiceInfo.h"

#define MAX_PACKET_LEN (64 * 1024)		// 数据包最大长度

GatewayConnection::GatewayConnection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback) : ServerConnection(connID, socket, closeCallback)
{

}

GatewayConnection::~GatewayConnection()
{
	
}

void GatewayConnection::parseMessage() {
	parsePacket();
}

// 协议数据包格式: 数据总长度(int)|msgId(int)|msg
void GatewayConnection::parsePacket()
{
	int dataLen = m_recvBuffer.size();
	while (dataLen > 0) {
		if (dataLen < 4) return;
		
		int packetLen = m_recvBuffer.getInt();
		if (packetLen < 8 || packetLen > MAX_PACKET_LEN) {
			Logger::logInfo("$packet len(%d) error", packetLen);
			close("packet format error");
			return;
		}
		// 当前数据长度小于协议包长度
		if (dataLen < packetLen) return;

		m_recvBuffer.readInt(); // 移除数据总长度字段
		int msgId = m_recvBuffer.readInt();
		int msgLen = packetLen - 8;
		dispatchClientMsg(msgId, msgLen, (char*)m_recvBuffer.data());
		m_recvBuffer.remove(msgLen);
		dataLen = m_recvBuffer.size();
		Logger::logInfo("$receive client msg, connId:%d, msgId:%d", getConnID(), msgId);
	}
}

void GatewayConnection::dispatchClientMsg(int msgId, int msgLen, const char* msgData) {
	MyBuffer buffer;
	buffer.writeByte(0);
	buffer.writeInt(getConnID());
	buffer.writeInt(msgId);
	buffer.writeString(msgData, msgLen);
	if (msgId == MSG_ID_LOGIN_REQ || msgId == MSG_ID_CREATE_ROLE_REQ || msgId == MSG_ID_ENTER_GAME) {
		ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_LOGIN, 0);
		CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	} else {
		if (m_sceneServiceId < 0) {
			Logger::logError("$player not in scene, connId:%d, msgId:%d", getConnID(), msgId);
			close("player not in scene");
			return;
		}
		ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_SCENE, m_sceneServiceId);
		CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	}
}

void GatewayConnection::sendMsgToService(int msgId, int msgLen, const char* msgData, ServiceAddr* addr) {
	MyBuffer buffer;
	buffer.writeByte(1);
	buffer.writeInt(getConnID()); // 统一格式
	buffer.writeInt(msgId);
	buffer.writeString(msgData, msgLen);
	
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(addr, (char*)buffer.data(), buffer.size());
}

void GatewayConnection::sendMsgToClient(int msgId, char* data, int dataLen) {
	int packetLen = dataLen + 8;
	MyBuffer buffer;
	buffer.writeInt(packetLen);
	buffer.writeInt(msgId);
	buffer.writeString(data, dataLen);
	send((char*)buffer.data(), buffer.size());
}

void GatewayConnection::setSceneServiceId(int sceneServiceId) {
	m_sceneServiceId = sceneServiceId;
	Logger::logDebug("$set secene service id, connId:%d, sceneServiceId:%d", getConnID(), sceneServiceId);
}