#pragma once

#include <string>
#include "GameActor.h"

class GamePlayer : public GameActor {
private:
	int m_connId;
	int m_roleId;
	std::string m_name;
	void* m_scriptObj;
public:
	GamePlayer(int connId, int actorId, int roleId, std::string name);
	inline void setScriptObj(void* scriptObj) { m_scriptObj = scriptObj; }
	void sendPacket(int msgId);
};