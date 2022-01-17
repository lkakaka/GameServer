#pragma once
#include <list>
#include <vector>
#include "boost/asio.hpp"
#include "Network.h"

using boost::asio::ip::tcp;

NS_GAME_NET_BEGIN

class ConnectionBase {
private:
	std::list<std::vector<unsigned char>> m_sendBuf; // �ȴ����͵��б�
	std::list<std::vector<unsigned char>> m_sendingBuf; // ���ڷ��͵��б�
	std::vector<boost::asio::const_buffer>m_sendingCBuffer;
	bool m_isSending;
	DWORD m_sendThreadId;
private:
	void _afterSend(size_t bytes_transferred);
	bool _prepareSendBuf();
protected:
	std::shared_ptr<tcp::socket> m_socket;
	bool m_isConnected;
	void _trySend();
public:
	ConnectionBase(std::shared_ptr<tcp::socket> socket, bool isConnected);
	inline std::shared_ptr<tcp::socket> getSocket() { return m_socket; }
	void send(std::vector<unsigned char>&& dat);
};

NS_GAME_NET_END

