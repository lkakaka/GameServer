#include "GameActor.h"

GameActor::GameActor(ActorType actorType, int actorId) : m_actorType(actorType), m_actorId(actorId), x(0), y(0), m_moveSpeed(0)
{

}

GameActor::GameActor(ActorType actorType, int actorId, int x, int y) : m_actorType(actorType), m_actorId(actorId), x(x), y(y), m_moveSpeed(0)
{

}

void GameActor::setPos(int x, int y) {
	this->x = x;
	this->y = y;
}
