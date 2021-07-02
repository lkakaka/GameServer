#include "TcpCommEntity.h"
#include "Logger.h"

extern const char* CONN_KEY;
INIT_SINGLETON_CLASS(TcpCommEntity)

TcpCommEntity::TcpCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int port) :
	IServiceCommEntity(addr),
	ClientConnection(io, serverIp, port)
{

}

void TcpCommEntity::onRecvData(std::vector<char>& data, int len) {
	m_recvBuf.append(data, len);
	_parse();
}

void TcpCommEntity::_parse() {
	int size = m_recvBuf.size();
	if (size < 16) return;
	
	int packetLen = m_recvBuf.getInt(12);
	if (packetLen > size - 16) return;

	int serviceGroup = m_recvBuf.readInt();
	int serviceType = m_recvBuf.readInt();
	int serviceId = m_recvBuf.readInt();
	packetLen = m_recvBuf.readInt(); // ��������
	ServiceAddr sender(serviceGroup, serviceType, serviceId);
	if (m_recvCallback != NULL) {
		m_recvCallback(&sender, (char*)m_recvBuf.data(), packetLen);
	}

	m_recvBuf.remove(packetLen);

	_parse();
}

void TcpCommEntity::sendToService(ServiceAddr* dstAddr, char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeInt(dstAddr->getServiceGroup());
	buffer.writeInt(dstAddr->getServiceType());
	buffer.writeInt(dstAddr->getServiceId());
	buffer.writeInt(msgLen);
	buffer.writeString(msg, msgLen);
	send((char*)buffer.data(), buffer.size());
	Logger::logInfo("$tcp comm send msg to service %s, len:%d!!!", dstAddr->getName()->c_str(), msgLen);
}

void TcpCommEntity::onConnect() {
	MyBuffer buffer;
	buffer.writeInt(addr.getServiceGroup());
	buffer.writeInt(addr.getServiceType());
	buffer.writeInt(addr.getServiceId());
	buffer.writeInt(strlen(CONN_KEY));
	int len = strlen(CONN_KEY);
	buffer.writeString(CONN_KEY, strlen(CONN_KEY));
	send((char*)buffer.data(), buffer.size());
	Logger::logInfo("$connected center service!!!");
}