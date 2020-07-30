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
	GamePlayer(int connId, int actorId, int roleId, std::string name, int x, int y, void* gameScene, GridChgFunc posChgFunc);
	inline void setScriptObj(void* scriptObj) { m_scriptObj = scriptObj; }
	void sendPacket(int msgId);

	inline int getConnId() { return m_connId; }

	bool onRecvClientMsg(int msgId, char* data, int dataLen);
};