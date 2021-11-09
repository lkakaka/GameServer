#pragma once

#include <string>
#include "SceneEntity.h"
#include "proto.h"

class PlayerEntity : public SceneEntity {
private:
	int m_connId;
	int m_roleId;
	std::string m_name;
	void* m_scriptObj;
public:
	PlayerEntity(int connId, int actorId, int roleId, std::string name, int x, int y, int moveSpeed, void* gameScene, GridChgFunc posChgFunc);
	inline void setScriptObj(void* scriptObj) { m_scriptObj = scriptObj; }

	inline int getConnId() { return m_connId; }
	void setConnId(int connId);

	void sendToClient(int msgId, const char* msg, int msgLen);
	void sendToClient(int msgId, google::protobuf::Message* msg);

	bool onRecvClientMsg(int msgId, char* data, int dataLen);
};