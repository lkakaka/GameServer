#include "HttpClientMgr.h"
#include "Logger.h"

INIT_SINGLETON_CLASS(HttpClientMgr)

HttpClientMgr::HttpClientMgr(boost::asio::io_service* io) : m_io(io)
{

}

void HttpClientMgr::init(boost::asio::io_service* io) {
	if (HttpClientMgr::_singleon != NULL) return;
	new HttpClientMgr(io);
}

HttpClient* HttpClientMgr::sendHttpReq(const std::string& server, const std::string& path, HTTP_CLIENT_CB callback) {
	HttpClient* client = new HttpClient(*m_io, server, path, callback);
	m_clients.emplace((int64_t)client, client);
	return client;
}

void HttpClientMgr::removeClient(HttpClient* client) {
	auto iter = m_clients.find((int64_t)client);
	if (iter == m_clients.end()) {
		LOG_ERROR("not found http client obj");
		return;
	}
	iter->second->close();
	delete iter->second;
	m_clients.erase((int64_t)client);
	LOG_INFO("remove http client");
}