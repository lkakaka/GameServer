#include "GamePlayer.h"


GamePlayer::GamePlayer(int connId, int actorId, int roleId, std::string name, int x, int y): GameActor(ActorType::PLYAER, actorId, x, y),
	m_connId(connId), m_roleId(roleId), m_name(name), m_scriptObj(NULL)
{
}

void GamePlayer::sendPacket(int msgId)
{

}

