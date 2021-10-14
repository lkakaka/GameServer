#include "LuaHttp.h"
#include "Logger.h"
#include "LuaRegistryObj.h"
#include "LuaPlugin.h"
#include "http_client/HttpClientMgr.h"


std::map<int, server*> LuaHttp::m_servers;

http::server::reply_ptr getHttpReply(sol::table resp) {
	http::server::reply_ptr rep = std::make_shared<http::server::reply>();
	int status = resp.get<int>("status");
	const char* body = resp.get<const char*>("body");
	rep->status = (http::server::reply::status_type)status;
	rep->content = body;

	if (resp["headers"].valid()) {
		sol::table headers = resp.get<sol::table>("headres");
		for (auto& kv : headers) {
			http::server::header h;
			sol::object key = kv.first;
			sol::object val = kv.second;
			h.name = key.as<const char*>();
			h.value = val.as<const char*>();
			rep->headers.emplace_back(h);
		}
	}
	return rep;
}

reply_ptr onRecvHttpReq(void* server, int conn_id, const http::server::request& req) {
	LuaPlugin::getSingleton()->addTask([server, conn_id, req]() {

		http::server::server* _server = (http::server::server*)server;
		connection_ptr conn = _server->get_connection_mgr()->get_connection(conn_id);
		if (conn == NULL) return;

		std::shared_ptr<sol::state> luaPtr = LuaPlugin::getSingleton()->getLua();
		sol::table scriptObj = luaPtr->registry()[_server->getLuaRef()];

		sol::table httpReq = sol::table::create_with(luaPtr->lua_state());
		httpReq["method"] = req.method.c_str();
		httpReq["uri"] = req.uri.c_str();
		sol::table headers = sol::table::create_with(luaPtr->lua_state());
		for (header h : req.headers) {
			headers[h.name.c_str()] = h.value.c_str();
		}
		httpReq["headers"] = headers;
		sol::function funcObj = scriptObj["on_recv_http_req"];
		sol::protected_function_result result = LuaPlugin::callLuaFunc(funcObj, scriptObj, conn_id, httpReq);
		if (!result.valid()) {
			LOG_ERROR("http error");
			_server->get_connection_mgr()->stop(conn);
			return;
		}

		if (result.pop_count() == 0) {
			LOG_ERROR("http not return");
			_server->get_connection_mgr()->stop(conn);
			return;
		}

		sol::table resp = result.get<sol::table>(0);

		reply_ptr rep = NULL;
		if (resp.valid()) {
			rep = getHttpReply(resp);
			conn->send_resp(rep);
		} else {
			_server->get_connection_mgr()->stop(conn);
		}
		//return rep;
	});
	return NULL;
}

static void sendHttpReq(const char* url, sol::function cb) {
	HttpClientMgr::getSingleton()->sendHttpReq(url, [cb](int http_code, std::string& resp) {
		std::shared_ptr<sol::state> luaPtr = LuaPlugin::getSingleton()->getLua();
		sol::table httpResp = sol::table::create_with(luaPtr->lua_state());
		httpResp["status"] = http_code;
		httpResp["content"] = resp.c_str();
		sol::protected_function_result result = cb(httpResp);
		if (!result.valid()) {
			LOG_ERROR("lua result = %d", result.status());
			sol::error err = result;
			std::string what = err.what();
			std::cout << what << std::endl;
			LOG_ERROR("%s", err.what());
		}
	});
}

void LuaHttp::bindLuaHttp(std::shared_ptr<sol::state> lua) {
	//sol::usertype<LuaHttpServer> luaHttp_type = lua->new_usertype<LuaHttpServer>("LuaHttpServer",
	//	// 3 constructors
	//	sol::constructors<LuaHttpServer(int)>());

	// typical member function that returns a variable

	sol::table httpServer = lua->create_named_table("HttpServer");
	httpServer["createHttpServer"] = &LuaHttp::createHttpServer;
	httpServer["stopHttpServer"] = &LuaHttp::stopHttpServer;

	sol::table httpClient = lua->create_named_table("HttpClient");
	httpClient["sendHttpReq"] = &sendHttpReq;
}

void LuaHttp::createHttpServer(int port, sol::table script, sol::this_state s) {
	char httpServerPort[8]{ 0 };
	sprintf(httpServerPort, "%d", port);
	http::server::server* serv = new http::server::server("0.0.0.0", httpServerPort, "");
	m_servers.emplace(port, serv);
	std::vector<int> regInfo = LuaRegistryObj::addRegistryObj(script);
	serv->bindLuaScriptObject(regInfo[0], regInfo[1], NULL);
	serv->setCallHttpScripFunc(onRecvHttpReq);
	
	//std::shared_ptr<std::thread> http_thread;
	//http_thread.reset(new std::thread([serv] { serv->run(); }));
	new std::thread([serv] { serv->run(); });
	//new std::thread([&serv] { serv->run(); });
	LOG_INFO("create http server, port:%d", port);
}

void LuaHttp::stopHttpServer(sol::table script, sol::this_state s) {

}
