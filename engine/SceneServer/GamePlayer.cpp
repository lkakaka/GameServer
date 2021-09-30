#include "GamePlayer.h"
#include "MyBuffer.h"
#include "Network/ServiceCommEntityMgr.h"
#include "ServiceType.h"
#include "ServiceInfo.h"
#include "Logger.h"
#include "../Common/ServerMacros.h"

GamePlayer::GamePlayer(int connId, int actorId, int roleId, std::string name, int x, int y, int moveSpeed, void* gameScene, GridChgFunc gridChgFunc):
	GameActor(ActorType::PLYAER, actorId, x, y, moveSpeed, gameScene, gridChgFunc),
	m_connId(connId), m_roleId(roleId), m_name(name), m_scriptObj(NULL)
{
}

void GamePlayer::sendToClient(int msgId, google::protobuf::Message* msg) {
	std::string msgData;
	msg->SerializeToString(&msgData);
	sendToClient(msgId, msgData.c_str(), msgData.length());
}

void GamePlayer::sendToClient(int msgId, const char* msg, int msgLen) {
	std::set<int> connIds;
	connIds.emplace(m_connId);
	broadcastMsgToClient(connIds, msgId, msg, msgLen);
}

bool GamePlayer::onRecvClientMsg(int msgId, char* data, int dataLen) {
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

