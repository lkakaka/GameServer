#pragma once

#include "sol/sol.hpp"

class LuaService {
public:
	static void bindLuaService(std::shared_ptr<sol::state>);
	static bool sendMsgToService(sol::table dstAddr, int msgId, const char* msg, int msgLen);
	static bool sendMsgToClient(int connId, int msgId, const char* msg, int msgLen);
	
};
