#include "GamePlayer.h"


GamePlayer::GamePlayer(int connId, int actorId, int roleId, std::string name): GameActor(actorId),
	m_connId(connId), m_roleId(roleId), m_name(name)
{
}

void GamePlayer::sendPacket(int msgId)
{

}

