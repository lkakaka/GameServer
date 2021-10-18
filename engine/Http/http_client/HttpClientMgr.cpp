#include "HttpClientMgr.h"
#include "Logger.h"
#include "AsioService.h"

#define HTTP_REQUEST_TIMEOUT 30

typedef struct _SocketInfo
{
	_SocketInfo(boost::asio::io_service& io)
		: sock(io),
		mask(0)
	{

	}

	boost::asio::ip::tcp::socket sock;
	int mask; // is used to store current action 
} SocketInfo;
typedef boost::shared_ptr<SocketInfo> socket_ptr;

// windows 上连接超时， eventcb 会一直挂起。需要在 closesocket 回调中取消所有异步事件
// 取消异步事件又会导致， eventcb 在 closesocket 回调 之后被调用。
// 所以不能简单的在 closesocket 回调 中释放 socket_ptr 的资源。所以这里得用 shared_ptr
std::map<curl_socket_t, socket_ptr> socket_map;

INIT_SINGLETON_CLASS(HttpClientMgr)

static void setsock(socket_ptr& tcp_socket, curl_socket_t s, CURL* e, int act, HttpClientMgr* mgr);

HttpClientMgr::HttpClientMgr(CURLM* curlm) : m_curlm(curlm), timer(boost::asio::deadline_timer(MAIN_IO))
{

}

static curl_socket_t opensocket(void* clientp, curlsocktype purpose, struct curl_sockaddr* address)
{
	LOG_DEBUG("opensocket :");

	curl_socket_t sockfd = CURL_SOCKET_BAD;

	/* restrict to ipv4 */
	if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
	{
		/* create a tcp socket object */
		//boost::asio::io_service* io_service = HttpClientMgr::getSingleton()->getIoService();
		SocketInfo* tcp_socket = new SocketInfo(MAIN_IO);

		/* open it and get the native handle*/
		boost::system::error_code ec;
		tcp_socket->sock.open(boost::asio::ip::tcp::v4(), ec);

		if (ec)
		{
			//An error occurred
			std::cout << std::endl << "Couldn't open socket [" << ec << "][" << ec.message() << "]";
			LOG_DEBUG("ERROR: Returning CURL_SOCKET_BAD to signal error");
		}
		else
		{
			sockfd = tcp_socket->sock.native_handle();
			LOG_DEBUG("Opened socket %d", sockfd);

			/* save it for monitoring */
			size_t size = socket_map.size();
			socket_map.insert(std::pair<curl_socket_t, socket_ptr>(sockfd, socket_ptr(tcp_socket)));
			assert(size + 1 == socket_map.size());
		}
	}

	return sockfd;
}

/* Die if we get a bad CURLMcode somewhere */
static void mcode_or_die(const char* where, CURLMcode code)
{
	if (CURLM_OK != code)
	{
		const char* s;
		switch (code)
		{
		case CURLM_CALL_MULTI_PERFORM: s = "CURLM_CALL_MULTI_PERFORM"; break;
		case CURLM_BAD_HANDLE:         s = "CURLM_BAD_HANDLE";         break;
		case CURLM_BAD_EASY_HANDLE:    s = "CURLM_BAD_EASY_HANDLE";    break;
		case CURLM_OUT_OF_MEMORY:      s = "CURLM_OUT_OF_MEMORY";      break;
		case CURLM_INTERNAL_ERROR:     s = "CURLM_INTERNAL_ERROR";     break;
		case CURLM_UNKNOWN_OPTION:     s = "CURLM_UNKNOWN_OPTION";     break;
		case CURLM_LAST:               s = "CURLM_LAST";               break;
		default: s = "CURLM_unknown";
			break;
		case     CURLM_BAD_SOCKET:         s = "CURLM_BAD_SOCKET";
			LOG_ERROR("ERROR: %s returns %s", where, s);
			/* ignore this error */
			return;
		}
		LOG_ERROR("ERROR: %s returns %s", where, s);
		//exit(code);
	}
}

/* Check for completed transfers, and remove their easy handles */
static void check_multi_info(HttpClientMgr* mgr)
{
	char* eff_url;
	CURLMsg* msg;
	int msgs_left;
	CURLRequest* request;
	CURL* easy;
	CURLcode res;

	LOG_DEBUG("REMAINING: %d", mgr->still_running);

	CURLM* mulit = mgr->getCURLM();

	while ((msg = curl_multi_info_read(mulit, &msgs_left)))
	{
		if (msg->msg == CURLMSG_DONE)
		{
			easy = msg->easy_handle;
			res = msg->data.result;
			curl_easy_getinfo(easy, CURLINFO_PRIVATE, &request);
			curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &eff_url);
			curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &request->resp_code);
			LOG_DEBUG("DONE: %s => (%d) %s", eff_url, res, request->error);
	        curl_easy_setopt(easy, CURLOPT_PRIVATE, NULL);
			curl_multi_remove_handle(mulit, easy);
			//free(request->url);
			curl_easy_cleanup(easy);
			if (request->callback != NULL) {
				if (strlen(request->error) > 0) {
					request->resp_content = request->error;
				}
				request->callback(request->resp_code, request->resp_content);
			}
			//LOG_INFO("recv: %s", request->resp_content.c_str());
			delete request;
		}
	}
}

/* Called by asio when there is an action on a socket */
static void event_cb(HttpClientMgr* mgr, socket_ptr& tcp_socket, curl_socket_t s, CURL* e, int action, const boost::system::error_code& err)
{
	CURLMcode rc;

	assert(tcp_socket->sock.native_handle() == s);

	CURLM* multi = mgr->getCURLM();
	if (err)
	{
		LOG_ERROR("event_cb: socket=%d action=%d \nERROR=%s", s, action, err.message().c_str());
		rc = curl_multi_socket_action(multi, tcp_socket->sock.native_handle(), CURL_CSELECT_ERR, &mgr->still_running);
	}
	else
	{
		LOG_DEBUG("event_cb: socket=%d action=%d", s, action);
		rc = curl_multi_socket_action(multi, tcp_socket->sock.native_handle(), action, &mgr->still_running);
	}

	mcode_or_die("event_cb: curl_multi_socket_action", rc);
	check_multi_info(mgr);

	if (mgr->still_running <= 0)
	{
		LOG_INFO("last transfer done, kill timeout");
		mgr->timer.cancel();
	}
	else
	{
		int action_continue = (tcp_socket->mask) & action;
		if (action_continue)
		{
			LOG_DEBUG("continue read or write: %d", action_continue);
			setsock(tcp_socket, s, e, action_continue, mgr); // continue read or write
		}
	}
}

static void setsock(socket_ptr& tcp_socket, curl_socket_t s, CURL* e, int act, HttpClientMgr* mgr)
{
	LOG_DEBUG("setsock: socket=%d, act=%d ", s, act);
	assert(tcp_socket->sock.native_handle() == s);

	if (act == CURL_POLL_IN)
	{
		LOG_DEBUG("watching for socket to become readable");

		tcp_socket->sock.async_read_some(boost::asio::null_buffers(),
			boost::bind(&event_cb, mgr,
				tcp_socket, s, e,
				act, _1));
	}
	else if (act == CURL_POLL_OUT)
	{
		LOG_DEBUG("watching for socket to become writable");

		tcp_socket->sock.async_write_some(boost::asio::null_buffers(),
			boost::bind(&event_cb, mgr,
				tcp_socket, s, e,
				act, _1));
	}
	else if (act == CURL_POLL_INOUT)
	{
		LOG_DEBUG("watching for socket to become readable & writable");

		tcp_socket->sock.async_read_some(boost::asio::null_buffers(),
			boost::bind(&event_cb, mgr,
				tcp_socket, s, e,
				CURL_POLL_IN, _1));

		tcp_socket->sock.async_write_some(boost::asio::null_buffers(),
			boost::bind(&event_cb, mgr,
				tcp_socket, s, e,
				CURL_POLL_OUT, _1));
	}
}


//callback by curl, socket be created
static int multi_sock_cb(CURL* e, curl_socket_t s, int what, void* cbp, void* sockp)
{
	LOG_DEBUG("multi_sock_cb: socket=%d, what=%d, sockp=%p", s, what, sockp);

	//GlobalInfo* g = (GlobalInfo*)cbp;

	int* actionp = (int*)sockp;
	const char* whatstr[] = { "none", "IN", "OUT", "INOUT", "REMOVE" };

	LOG_DEBUG("socket callback: s=%d e=%p what=%s ", s, e, whatstr[what]);

	std::map<curl_socket_t, socket_ptr>::iterator it = socket_map.find(s);

	if (it == socket_map.end())
	{
		if (actionp)
		{
			LOG_DEBUG("socket closed already before remove CURL_POLL_REMOVE event. bug???", s);
			//*actionp = what;
			return 0;
		}
		else
		{
			LOG_DEBUG("we don't know how to create asio::ip::tcp::socket without this fd's protocol family, please recompiled libcurl without c-ares\n");
			LOG_DEBUG("socket %d is a c-ares socket, ignoring", s);
			return 0; // don't poll this fd, will cause c-ares read dns response until timeout
		}

	}

	socket_ptr& tcp_socket = it->second;

	if (!actionp)
	{
		actionp = &(tcp_socket->mask);

		curl_multi_assign(HttpClientMgr::getSingleton()->getCURLM(), s, actionp);
	}

	if (what == CURL_POLL_REMOVE)
	{
		LOG_DEBUG("remsock: socket=%d", s);
	}
	else
	{
		LOG_DEBUG("Changing action from %s to %s", whatstr[*actionp], whatstr[what]);
		setsock(tcp_socket, s, e, what & (~*actionp), HttpClientMgr::getSingleton()); // only add new instrest
	}

done:
	*actionp = what;
	return 0;
}

/* CURLOPT_CLOSESOCKETFUNCTION */
static int closesocket_cb(void* clientp, curl_socket_t item)
{
	LOG_DEBUG("closesocket : %d", item);

	std::map<curl_socket_t, socket_ptr>::iterator it = socket_map.find(item);

	if (it != socket_map.end())
	{
		assert(it->second->sock.native_handle() == item);
		it->second->mask = 0;
		// close or cancel will cancel any asynchronous send, receive or connect operations 
		// Caution: on Windows platform, if connect host timeout, the event_cb will pending forever. Must be canceled manually
		it->second->sock.cancel();
		socket_map.erase(it);
	}
	else
		assert(false);

	return 0;
}

/* Called by asio when our timeout expires */
static void timer_cb(const boost::system::error_code& error)
{
	if (!error)
	{
		LOG_ERROR("timer_cb: ");

		CURLMcode rc;
		HttpClientMgr* mgr = HttpClientMgr::getSingleton();
		rc = curl_multi_socket_action(mgr->getCURLM(), CURL_SOCKET_TIMEOUT, 0, &mgr->still_running);

		mcode_or_die("timer_cb: curl_multi_socket_action", rc);
		check_multi_info(mgr);
	}
}

/* Update the event timer after curl_multi library calls */
static int multi_timer_cb(CURLM* multi, long timeout_ms)
{
	LOG_DEBUG("multi_timer_cb: timeout_ms %ld", timeout_ms);

	/* cancel running timer */
	HttpClientMgr::getSingleton()->timer.cancel();

	if (timeout_ms > 0)
	{
		/* update timer */
		HttpClientMgr::getSingleton()->timer.expires_from_now(boost::posix_time::millisec(timeout_ms));
		HttpClientMgr::getSingleton()->timer.async_wait(boost::bind(&timer_cb, _1));
	}
	else
	{
		/* call timeout function immediately */
		boost::system::error_code error; /*success*/
		timer_cb(error);
	}

	return 0;
}


void HttpClientMgr::init() {
	if (HttpClientMgr::_singleon != NULL) return;
	curl_version_info_data* curl_version = curl_version_info(CURLversion::CURLVERSION_FIRST);
	LOG_INFO("curl version: %s", curl_version->version);
	curl_global_init(CURL_GLOBAL_ALL);
	CURLM* curlm = curl_multi_init();
	curl_multi_setopt(curlm, CURLMOPT_SOCKETFUNCTION, multi_sock_cb);
	curl_multi_setopt(curlm, CURLMOPT_TIMERFUNCTION, multi_timer_cb);
	HttpClientMgr* mgr = new HttpClientMgr(curlm);
}

static size_t on_curl_write_cb(char* ptr, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	CURLRequest* req = (CURLRequest*)userp;
	std::copy(ptr, ptr + realsize, std::back_inserter(req->resp_content));
	return realsize;
}

void HttpClientMgr::sendHttpReq(const char* url, HTTP_CLIENT_CB callback) {
	/*HttpClient* client = new HttpClient(*m_io, server, path, callback);
	m_clients.emplace((int64_t)client, client);*/
	CURLRequest* request = new CURLRequest();
	request->callback = callback;
	
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, on_curl_write_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, request);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)HTTP_REQUEST_TIMEOUT);
	curl_easy_setopt(curl, CURLOPT_PRIVATE, request);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, request->error);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L); // timeout for the connect phase 
	/* call this function to get a socket */
	curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, opensocket);
	/* call this function to close a socket */
	curl_easy_setopt(curl, CURLOPT_CLOSESOCKETFUNCTION, closesocket_cb);

	CURLMcode rc = curl_multi_add_handle(m_curlm, curl);
	mcode_or_die("new_conn: curl_multi_add_handle", rc);

	/* Now specify the POST data */
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");
	/*const char* request = "GET /1?cmd=gm_list HTTP/1.0\r\nHost: localhost:8028\r\n\r\n";
	size_t request_len = strlen(request);*/

	//curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);


	/* Perform the request, res will get the return code */
	//CURLcode res = curl_easy_perform(curl);
	///* Check for errors */
	//if (res != CURLE_OK) {
	//	LOG_ERROR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	//}
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
