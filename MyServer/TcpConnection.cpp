#include "TcpConnection.h"
#include "proto/test.pb.h"
#include "proto/proto.h"

TcpConnection::TcpConnection(boost::asio::io_service& io, int connID, closeFuncType closeFunc):
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
		Log::logError("socket close error, %s", e.what());
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

inline int parseIntFromData(unsigned char* data) {
	return *(int*)data;
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
			Log::logError("$close connection, %s", err_str.data());
			m_closeFunc(getConnID());
			return;
		}
		if (datLen > 0)
		{
			Log::logInfo("$receive data, len:%d, %s\n", datLen, m_vecData.data());

			/*std::string echo = "server echo:";
			std::vector<unsigned char>data;
			std::copy(echo.begin(), echo.end(), std::back_inserter(data));*/
			std::copy(m_vecData.begin(), m_vecData.end(), std::back_inserter(m_readData));
			if (m_vecData.size() >= 8) {
				unsigned char* data = m_vecData.data();
				int msgLen = parseIntFromData(&data[4]);
				if(m_vecData.size() - 8 >= msgLen){ 
					int msgId = parseIntFromData(data);
					google::protobuf::Message* msg = (google::protobuf::Message*)CreateMsgById(msgId);
					msg->ParseFromArray(&data[8], msgLen);
					std::vector<unsigned char>::iterator end = m_vecData.begin();
					std::advance(m_vecData.begin(), msgLen + 8);
					m_vecData.erase(m_vecData.begin(), end);
				}
			}

			/*Test recvMsg;
			recvMsg.ParseFromArray(m_vecData.data(), datLen);
			Log::logInfo("$receive data obj, id:%d, msg:%s\n", recvMsg.id(), recvMsg.msg().data());*/

			Test msg;
			msg.set_id(1);
			msg.set_msg("hello");
			std::string echo;
			msg.SerializeToString(&echo);
			
			std::vector<unsigned char>data;
			std::copy(echo.begin(), echo.end(), std::back_inserter(data));
			sendData(std::move(data), data.size());
		}
		else {
			Log::logInfo("$receive data len is 0");
		}
		this->doRead();
	});
}

void TcpConnection::sendData(std::vector<unsigned char>&& dat, size_t datLen)
{
	boost::asio::const_buffer buf(&dat.front(), datLen);
	m_socket.async_write_some(buf, [](boost::system::error_code err_code, size_t datLen) {
		Log::logInfo("$send data len:%d", datLen);
	});
}

void TcpConnection::doShutDown()
{
	try {
		this->m_socket.shutdown(m_socket.shutdown_both);
	}
	catch (boost::system::system_error e) {
		Log::logError("socket shutdown error, %s", e.what());
	}
}