#include "PlayerEntity.h"
#include "MyBuffer.h"
#include "Network/ServiceCommEntityMgr.h"
#include "ServiceType.h"
#include "Logger.h"
#include "../Common/ServerMacros.h"
#include "GameScene.h"

PlayerEntity::PlayerEntity(int connId, int eid, int64_t roleId, std::string name, int x, int y, int moveSpeed, void* gameScene, GridChgFunc gridChgFunc):
	SceneEntity(SceneEntityType::PLYAER, eid, x, y, moveSpeed, gameScene, gridChgFunc),
	m_connId(connId), m_roleId(roleId), m_name(name), m_scriptObj(NULL)
{
}

void PlayerEntity::sendToClient(int msgId, google::protobuf::Message* msg) {
	std::string msgData;
	msg->SerializeToString(&msgData);
	sendToClient(msgId, msgData.c_str(), msgData.length());
}

void PlayerEntity::sendToClient(int msgId, const char* msg, int msgLen) {
	std::set<int> connIds;
	connIds.emplace(m_connId);
	broadcastMsgToClient(connIds, msgId, msg, msgLen);
}

void PlayerEntity::setConnId(int connId) {
	if (m_connId == connId) return;
	((GameScene*)m_gameScene)->changePlayerConnId(m_connId, connId);
	m_connId = connId;
}

bool PlayerEntity::onRecvClientMsg(int msgId, char* data, int dataLen) {
	switch (msgId)
	{
		case MSG_ID_MOVE_TO: {
			std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, (char*)data, dataLen);
			MoveTo* moveMsg = (MoveTo*)msg.get();
			std::vector<Position> posList;
			posList.emplace_back(moveMsg->pos_x(), moveMsg->pos_y());
			setTgtPosList(posList);
			//setPos(moveMsg->pos_x(), moveMsg->pos_y());
			break;
		}
		default:
			break;
	}
	return true;
}

