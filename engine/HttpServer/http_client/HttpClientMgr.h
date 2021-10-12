#pragma once

#include "Singleton.h"
#include "http_client.h"

class HttpClientMgr : public Singleton<HttpClientMgr>
{
private:
	boost::asio::io_service* m_io;
	std::map<int, HttpClient*> m_clients;
public:
	static void init(boost::asio::io_service* io);
	HttpClientMgr(boost::asio::io_service* io);
	HttpClient* sendHttpReq(const std::string& server, const std::string& path, HTTP_CLIENT_CB callback);
	void removeClient(HttpClient* client);
};

