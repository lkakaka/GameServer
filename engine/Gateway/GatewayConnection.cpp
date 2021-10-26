#include "GatewayConnection.h"
#include "proto.h"
#include "Network/ServiceCommEntityMgr.h"
#include "ServiceType.h"
#include "ServiceInfo.h"
#include "Network/Network.h"
#include "Timer.h"
#include "TimeUtil.h"
#include "GatewayNet.h"
#include "proto.h"


#define KCP_TOKEN_VALID_TIME 30000 // 毫秒

using boost::asio::ip::udp;

GatewayConnection::GatewayConnection(int connID, std::shared_ptr<tcp::socket> socket, ConnCloseCallback closeCallback) :
	ServerConnection(connID, socket, closeCallback), m_kcp(NULL), 
	m_kcpTimerId(0), m_kcpEnabled(false), m_kcpTokenTs(0), m_clientUdpPort(0)
{
	
}

GatewayConnection::~GatewayConnection()
{
	stopKCP();
}

void GatewayConnection::startKCP() {
	if (m_kcp != NULL) return;
	int connId = getConnID();
	m_kcp = ikcp_create(connId, this);
	m_kcp->output = GatewayConnection::udp_output;
	m_kcpTimerId = TimerMgr::getSingleton()->addTimer(0, 10, -1, [this](int timerId) {
		// 以一定频率调用 ikcp_update来更新 kcp状态，并且传入当前时钟（毫秒单位）
		// 如 10ms调用一次，或用 ikcp_check确定下次调用 update的时间不必每次调用
		ikcp_update(m_kcp, TimeUtil::nowMillSec());
	});
	LOG_INFO("start kcp, connId:%d", connId);
}

void GatewayConnection::stopKCP() {
	if (m_kcpTimerId > 0) {
		TimerMgr::getSingleton()->removeTimer(m_kcpTimerId, true);
		m_kcpTimerId = 0;
	}
	if (m_kcp != NULL) {
		ikcp_release(m_kcp);
		m_kcp = NULL;
		LOG_INFO("stop kcp, connId:%d", getConnID());
	}
}

// KCP的下层协议输出函数，KCP需要发送数据时会调用它
// buf/len 表示缓存和长度
// user指针为 kcp对象创建时传入的值，用于区别多个 KCP对象
int GatewayConnection::udp_output(const char* buf, int len, ikcpcb * kcp, void* user)
{
	LOG_INFO("kcp output, len:%d", len);
	GatewayConnection* gcPtr = (GatewayConnection*)user;
	try {
		udp::socket* udp_sock = GatewayNet::getSingleton()->getUpdSocket();
		auto sbuf = boost::asio::buffer(buf, len);
		const char* ip = gcPtr->getClientUdpIP();
		int port = gcPtr->getClientUdpPort();
		udp_sock->send_to(sbuf, udp::endpoint(boost::asio::ip::address_v4::from_string(ip), port));
		LOG_INFO("send kcp data, ip:%s, port:%d", ip, port);
	} catch (std::exception& e) {
		LOG_ERROR("udp data send error, %s", e.what());
	}
	return 1;
}

void GatewayConnection::enableKCP(std::string& token) { 
	m_kcpEnabled = true;
	m_kcpToken = token;
	m_kcpTokenTs = TimeUtil::nowMillSec() + KCP_TOKEN_VALID_TIME;
}

bool GatewayConnection::isKcpTokenValid(std::string& token) {
	if (!m_kcpEnabled) return false;
	if (TimeUtil::nowMillSec() >= m_kcpTokenTs) return false;
	return m_kcpToken.compare(token.c_str()) == 0;
}

void GatewayConnection::onRecvKCPMsg(const char* data, int len) {
	if (m_kcp == NULL) return;
	ikcp_input(m_kcp, data, len);
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
		if (packetLen < 8 || packetLen > MAX_CLIENT_PACKET_LEN) {
			LOG_ERROR("packet len(%d) error", packetLen);
			setWaitClose("packet format error");
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
		LOG_INFO("receive client msg, connId:%d, msgId:%d", getConnID(), msgId);
	}
}

void GatewayConnection::dispatchClientMsg(int msgId, int msgLen, const char* msgData) {
	MyBuffer buffer;
	buffer.writeByte(0);
	buffer.writeInt(getConnID());
	buffer.writeInt(msgId);
	buffer.writeString(msgData, msgLen);
	switch (msgId)
	{
		case MSG_ID_LOGIN_REQ:
		case MSG_ID_CREATE_ROLE_REQ:
		case MSG_ID_ENTER_GAME: {
			ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_LOGIN, 0);
			CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
			break;
		}
		default: {
			if (m_sceneServiceId < 0) {
				LOG_ERROR("player not in scene, connId:%d, msgId:%d", getConnID(), msgId);
				setWaitClose("player not in scene");
				return;
			}
			ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_SCENE, m_sceneServiceId);
			CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
			break;
		}
	}
}

void GatewayConnection::sendMsgToClient(send_type type, int msgId,  char* data, int dataLen) {
	int packetLen = dataLen + 8;
	MyBuffer buffer;
	buffer.writeInt(packetLen);
	buffer.writeInt(msgId);
	buffer.writeString(data, dataLen);
	if (type == SEND_TYPE_KCP) {
		if (m_kcp == NULL) {
			LOG_ERROR("send msg to client fail, kcp not start!!, connId:%d", getConnID());
			return;
		}
		ikcp_send(m_kcp, (char*)buffer.data(), buffer.size());
	}
	else {
		send(std::move(buffer.getBuf()));
	}
}

void GatewayConnection::setSceneServiceId(int sceneServiceId) {
	m_sceneServiceId = sceneServiceId;
	LOG_DEBUG("set secene service id, connId:%d, sceneServiceId:%d", getConnID(), sceneServiceId);
}