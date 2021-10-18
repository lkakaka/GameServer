#pragma once

#include "Singleton.h"
#include "http_client.h"
#include "curl/curl.h"

typedef struct _CURLRequest {
	bool is_done;
	int resp_code;
	char error[CURL_ERROR_SIZE]{0};
	// 放在error变量前定义，运行时会跑段错误,为什么??
    std::string resp_content;

	HTTP_CLIENT_CB callback;

	_CURLRequest() : is_done(false), resp_code(-1), callback(NULL) {

	}

	//boost::asio::ip::tcp::socket sock;
	//int mask; // is used to store current action 
}CURLRequest;

//typedef struct _CURLRequest
//{
//	_CURLRequest(boost::asio::io_service& io)
//		: sock(io),
//		mask(0)
//	{
//
//	}
//
//	boost::asio::ip::tcp::socket sock;
//	int mask; // is used to store current action 
//} CURLRequest;
//typedef boost::shared_ptr<CURLRequest> request_ptr;

class HttpClientMgr : public Singleton<HttpClientMgr>
{
private:
	std::map<int, HttpClient*> m_clients;
	CURLM* m_curlm;
	std::map<curl_socket_t, CURLRequest*> m_requests;

	HttpClientMgr(CURLM* curlm);

public:
	int still_running;
	boost::asio::deadline_timer timer;

	static void init();
	inline CURLM* getCURLM() { return m_curlm; }
	
	void sendHttpReq(const char* url, HTTP_CLIENT_CB callback);
	void removeClient(HttpClient* client);
};



