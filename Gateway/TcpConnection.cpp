#include "TcpConnection.h"
#include "MessageHandler.h"
#include "proto.h"
#include "ZmqInst.h"

#define MAX_PACKET_LEN (64 * 1024)		// 数据包最大长度

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
			std::string reason;
			const std::string err_str = error.message();
			//Logger::logError("$close connection, %s", err_str.data());
			reason = "read error," + err_str;
			close(reason.c_str());
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
		
		int packetLen = m_readBuf.readInt();
		if (packetLen < 8 || packetLen > MAX_PACKET_LEN) {
			Logger::logInfo("$packet len(%d) error", packetLen);
			close("packet format error");
			return;
		}
		// 当前数据长度小于协议包长度
		if (dataLen < packetLen) return;

		m_readBuf.remove(4); // 移除数据总长度字段
		int msgId = m_readBuf.readIntEx();
		int msgLen = packetLen - 8;
		dispatchMsg(msgId, msgLen, m_readBuf.data());
		m_readBuf.remove(msgLen);
		dataLen = m_readBuf.size();
		Logger::logInfo("$receive client msg, connId:%d, msgId:%d", m_connID, msgId);
	}
}

void TcpConnection::dispatchMsg(int msgId, int msgLen, const char* msgData) {
	MyBuffer buffer;
	buffer.writeInt(m_connID);
	buffer.writeInt(msgId);
	buffer.writeString(msgData, msgLen);
	if (msgId == MSG_ID_LOGIN_REQ) {
		ZmqInst::getZmqInstance()->sendData("login", buffer.data(), buffer.size());
	} else {
		ZmqInst::getZmqInstance()->sendData("scene", buffer.data(), buffer.size());
	}
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
	std::copy(dat.begin(), dat.end(), std::back_inserter(m_sendBuf));
	doSend();
}

void TcpConnection::doSend() {
	if (m_sendBuf.size() == 0) return;
	boost::asio::const_buffer buf(&m_sendBuf.front(), m_sendBuf.size());
	m_socket.async_write_some(buf, [this](boost::system::error_code err_code, size_t datLen) {
		if (err_code)
		{
			const std::string err_str = err_code.message();
			Logger::logError("$send data error, %s", err_str.data());
			//Logger::logInfo("$send data len:%d", datLen);
			return;
		}
		if (datLen > 0) {
			m_sendBuf.erase(m_sendBuf.begin(), m_sendBuf.begin() + datLen);
			if(m_sendBuf.size() > 0) doSend();
		}
	});
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

	Disconnect msg;
	msg.set_reason(reason);
	std::string msgData;
	msg.SerializeToString(&msgData);
	dispatchMsg(MSG_ID_DISCONNECT, msgData.length(), msgData.c_str());
}