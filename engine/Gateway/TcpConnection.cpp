#include "TcpConnection.h"
#include "proto.h"
#include "ZmqInst.h"
#include "ServiceType.h"
#include "ServiceInfo.h"

#define MAX_PACKET_LEN (64 * 1024)		// 数据包最大长度

//ZmqInst* Singleton<ZmqInst>::_singleon;

TcpConnection::TcpConnection(boost::asio::io_service& io, int connID, ConnCloseFunc closeFunc):
	m_isClosed(false),
	m_connID(connID),
	m_socket(io),
	m_closeFunc(closeFunc)
{

}

TcpConnection::~TcpConnection()
{
	/*try {
		this->m_socket.close();
	}catch (boost::system::system_error e) {
		Logger::logError("socket close error, %s", e.what());
	}*/
	//printf("delete TcpConnection\n");
}

tcp::socket& TcpConnection::getSocket()
{
	return m_socket;
}

int TcpConnection::getConnID() const
{
	return m_connID;
}

void TcpConnection::doRead()
{	
	m_vecData.resize(1024);
	m_vecData.assign(m_vecData.size(), 0);
	auto buf = boost::asio::buffer(m_vecData, m_vecData.size());
	m_socket.async_receive(buf, [buf, this](const boost::system::error_code& error, size_t bytes_transferred) {
		if (error)
		{
			const std::string err_str = error.message();
			Logger::logError("$close connection, %s", err_str.c_str());
			close("client disconnected");
			return;
		}
		if (bytes_transferred > 0)
		{
			//Logger::logInfo("$receive data, len:%d, %s", datLen, m_vecData.data());
			m_readBuf.append(m_vecData, bytes_transferred);
			parsePacket();
		}
		else {
			Logger::logInfo("$receive data len is 0");
		}
		if (!m_isClosed) {
			this->doRead();
		}
	});
}

// 协议数据包格式: 数据总长度(int)|msgId(int)|msg
void TcpConnection::parsePacket()
{
	int dataLen = m_readBuf.size();
	while (dataLen > 0) {
		if (dataLen < 4) return;
		
		int packetLen = m_readBuf.getInt();
		if (packetLen < 8 || packetLen > MAX_PACKET_LEN) {
			Logger::logInfo("$packet len(%d) error", packetLen);
			close("packet format error");
			return;
		}
		// 当前数据长度小于协议包长度
		if (dataLen < packetLen) return;

		m_readBuf.readInt(); // 移除数据总长度字段
		int msgId = m_readBuf.readInt();
		int msgLen = packetLen - 8;
		dispatchClientMsg(msgId, msgLen, (char*)m_readBuf.data());
		m_readBuf.remove(msgLen);
		dataLen = m_readBuf.size();
		Logger::logInfo("$receive client msg, connId:%d, msgId:%d", m_connID, msgId);
	}
}

void TcpConnection::dispatchClientMsg(int msgId, int msgLen, const char* msgData) {
	MyBuffer buffer;
	buffer.writeByte(0);
	buffer.writeInt(m_connID);
	buffer.writeInt(msgId);
	buffer.writeString(msgData, msgLen);
	if (msgId == MSG_ID_LOGIN_REQ || msgId == MSG_ID_CREATE_ROLE_REQ || msgId == MSG_ID_ENTER_GAME) {
		//ZmqInst::getZmqInstance()->sendData("login", (char*)buffer.data(), buffer.size());
		ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_LOGIN, 0);
		ZmqInst::getZmqInstance()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	} else {
		//ZmqInst::getZmqInstance()->sendData("scene", (char*)buffer.data(), buffer.size());

		ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_SCENE, 0);
		ZmqInst::getZmqInstance()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	}
}

void TcpConnection::sendMsgToService(int msgId, int msgLen, const char* msgData, ServiceAddr* addr) {
	MyBuffer buffer;
	buffer.writeByte(1);
	buffer.writeInt(m_connID); // 统一格式
	buffer.writeInt(msgId);
	buffer.writeString(msgData, msgLen);
	//ZmqInst::getZmqInstance()->sendData(serviceName, (char*)buffer.data(), buffer.size());

	ZmqInst::getZmqInstance()->sendToService(addr, (char*)buffer.data(), buffer.size());
}

void TcpConnection::sendMsgToClient(int msgId, char* data, int dataLen) {
	int packetLen = dataLen + 8;
	MyBuffer buffer;
	buffer.writeInt(packetLen);
	buffer.writeInt(msgId);
	buffer.writeString(data, dataLen);
	std::copy(buffer.data(), buffer.data() + buffer.size(), std::back_inserter(m_sendBuf));
	doSend();
}

void TcpConnection::sendData(std::vector<char>& dat)
{
	std::vector<char> buf;
	std::copy(dat.begin(), dat.end(), std::back_inserter(m_sendBuf));
	doSend();
}

void TcpConnection::doSend() {
	if (m_sendBuf.size() == 0) return;
	//m_sendMutex.lock();
	boost::asio::const_buffer buf(&m_sendBuf.front(), m_sendBuf.size());
	/*std::shared_ptr<std::vector<char>> buf_ptr = std::make_shared<std::vector<char>>();*/
	size_t len = m_socket.write_some(buf);
	if (len > 0) {
		m_sendBuf.erase(m_sendBuf.begin(), m_sendBuf.begin() + len);
		if(m_sendBuf.size() > 0) doSend();
	}
	//m_socket.async_write_some(buf, [this](boost::system::error_code err_code, size_t datLen) {
	//	//m_sendMutex.unlock();
	//	if (err_code)
	//	{
	//		const std::string err_str = err_code.message();
	//		Logger::logError("$send data error, %s", err_str.data());
	//		//Logger::logInfo("$send data len:%d", datLen);
	//		return;
	//	}
	//	if (datLen > 0) {
	//		m_sendBuf.erase(m_sendBuf.begin(), m_sendBuf.begin() + datLen);
	//		if(m_sendBuf.size() > 0) doSend();
	//	}
	//});
}

void TcpConnection::close(const char* reason) {
	m_closeFunc(getConnID(), reason);
}

void TcpConnection::doShutDown(const char* reason)
{
	if (m_isClosed) return;
	m_isClosed = true;
	try {
		this->m_socket.shutdown(m_socket.shutdown_both);
	}
	catch (boost::system::system_error e) {
		Logger::logError("socket shutdown error, %s", e.what());
	}

	ClientDisconnect msg;
	msg.set_conn_id(m_connID);
	msg.set_reason(reason);
	std::string msgData;
	msg.SerializeToString(&msgData);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_SCENE, 0);
	sendMsgToService(MSG_ID_CLIENT_DISCONNECT, msgData.length(), msgData.c_str(), &addr);
}