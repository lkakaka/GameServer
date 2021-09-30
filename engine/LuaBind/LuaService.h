#pragma once

#include "sol/sol.hpp"
#include <set>

class LuaService {
public:
	static void bindLuaService(std::shared_ptr<sol::state>);
	static bool sendMsgToService(sol::table dstAddr, int msgId, const char* msg, int msgLen);
	static bool sendMsgToClient(int connId, int msgId, const char* msg, int msgLen);
	static bool broadcastMsgToClient(std::set<int> connIds, int msgId, const char* msg, int msgLen);
	static bool sendMsgToClientKCP(int connId, int msgId, const char* msg, int msgLen);
};
