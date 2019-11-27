#pragma once

#include <string>
#include "GameActor.h"

class GamePlayer : public GameActor {
private:
	int m_connId;
	std::string name;
public:

	void sendPacket(int msgId);
};