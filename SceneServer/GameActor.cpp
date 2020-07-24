#include "GameActor.h"

GameActor::GameActor(ActorType actorType, int actorId, PosChgFunc posChgFunc) :
	m_actorType(actorType), m_actorId(actorId), m_pos(Position(0, 0)), m_moveSpeed(0),
	m_posChgFunc(posChgFunc)
{
	
}

GameActor::GameActor(ActorType actorType, int actorId, int x, int y, PosChgFunc posChgFunc) :
	m_actorType(actorType), m_actorId(actorId), m_pos(Position(x, y)), m_moveSpeed(0),
	m_posChgFunc(posChgFunc)
{

}

void GameActor::setPos(int x, int y) {
	this->m_pos.x = x;
	this->m_pos.y = y;
	m_posChgFunc(m_actorId, &Position(x, y));
}

void GameActor::setTgtPosList(std::vector<Position> tgtPosList) {
	if (!m_tgtPosList.empty()) {
		updatePos();
	}
	m_tgtPosList = tgtPosList;
}

void GameActor::updatePos() {

}
