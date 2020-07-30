#include "GamePlayer.h"
#include "proto.h"


GamePlayer::GamePlayer(int connId, int actorId, int roleId, std::string name, int x, int y, void* gameScene, GridChgFunc gridChgFunc): 
	GameActor(ActorType::PLYAER, actorId, x, y, gameScene, gridChgFunc),
	m_connId(connId), m_roleId(roleId), m_name(name), m_scriptObj(NULL)
{
}

void GamePlayer::sendPacket(int msgId)
{

}

bool GamePlayer::onRecvClientMsg(int msgId, char* data, int dataLen) {
	switch (msgId)
	{
		case MSG_ID_MOVE_TO: {
			std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
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

