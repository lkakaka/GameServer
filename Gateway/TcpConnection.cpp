#include "TcpConnection.h"
#include "MessageHandler.h"
#include "proto.h"
#include "ZmqInst.h"

TcpConnection::TcpConnection(boost::asio::io_service& io, int connID, ConnCloseFunc closeFunc):
	m_connID(connID),
	m_socket(io),
	m_closeFunc(closeFunc)
{

}

TcpConnection::~TcpConnection()
{
	try {
		this->m_socket.close();
	}catch (boost::system::system_error e) {
		Logger::logError("socket close error, %s", e.what());
	}
	printf("delete TcpConnection\n");
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
	m_socket.async_receive(buf, [buf, this](const boost::system::error_code& error, size_t datLen) {
		if (error)
		{
			const std::string err_str = error.message();
			Logger::logError("$close connection, %s", err_str.data());
			m_closeFunc(getConnID(), "client disconnect");
			return;
		}
		if (datLen > 0)
		{
			//Logger::logInfo("$receive data, len:%d, %s", datLen, m_vecData.data());
			auto iter = m_vecData.begin();
			std::advance(iter, datLen);
			std::copy(m_vecData.begin(), iter, std::back_inserter(m_readData));
			parseRecvData();
		}
		else {
			Logger::logInfo("$receive data len is 0");
		}
		this->doRead();
	});
}

void TcpConnection::parseRecvData()
{
	int len = 0;
	do {
		len = parseProtoData();
		if (len > 0) {
			auto removeIter = m_readData.begin();
			std::advance(removeIter, len);
			m_readData.erase(m_readData.begin(), removeIter);
		}
	} while (len > 0);
}

int TcpConnection::parseProtoData()
{
	char* data = m_readData.data();
	int dataLen = m_readData.size();
	if (dataLen < 8) {
		return 0;
	}
	int msgLen = readInt(&data[4]);
	if (dataLen - 8 < msgLen) {
		return 0;
	}
	int msgId = readInt(data);
	dispatchMsg(msgId, msgLen, data + 8);
	Logger::logInfo("$receive client msg, connId:%d, msgId:%d", m_connID, msgId);
	return msgLen + 8;
}

void TcpConnection::dispatchMsg(int msgId, int msgLen, const char* msgData) {
	if (msgId == MSG_ID_LOGIN_REQ) {

	}
	std::vector<char> tmp;
	writeIntEx(&tmp, m_connID);
	writeIntEx(&tmp, msgId);
	std::copy(msgData, msgData + msgLen, std::back_inserter(tmp));
	ZmqInst::getZmqInstance()->sendData("scene", tmp.data(), msgLen + 8);
}

void TcpConnection::sendMsgToClient(int msgId, char* data, int dataLen) {
	int msgLen = dataLen + 8;
	std::vector<char> buff;
	writeInt(&buff, msgLen);
	writeInt(&buff, msgId);
	std::copy(data, data + dataLen, std::back_inserter(buff));
	sendData(std::move(buff), buff.size());
}

void TcpConnection::sendData(std::vector<char>&& dat, size_t datLen)
{
	boost::asio::const_buffer buf(&dat.front(), datLen);
	m_socket.async_write_some(buf, [](boost::system::error_code err_code, size_t datLen) {
		if (err_code)
		{
			const std::string err_str = err_code.message();
			Logger::logError("$send data error, %s", err_str.data());
			//Logger::logInfo("$send data len:%d", datLen);
		}
	});
}

void TcpConnection::doShutDown(const char* reason)
{
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