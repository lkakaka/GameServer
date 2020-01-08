#pragma once

#include <string>
#include "GameActor.h"

class GamePlayer : public GameActor {
private:
	int m_connId;
	int m_roleId;
	std::string m_name;
public:
	GamePlayer(int connId, int actorId, int roleId, std::string name);
	void sendPacket(int msgId);
};