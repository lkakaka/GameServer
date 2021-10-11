#pragma once

#include "sol/sol.hpp"
#include "server.hpp"
#include <map>

using namespace http::server;

class LuaHttpServer {
private:
	static std::map<int, http::server::server*> m_servers;
public:
	static void bindLuaHttpServer(std::shared_ptr<sol::state> lua);
	static void createHttpServer(int port, sol::table script, sol::this_state s);
	static void stopHttpServer(sol::table script, sol::this_state s);
};
