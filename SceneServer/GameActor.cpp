#include "GameActor.h"
#include "TimeUtil.h"
#include <algorithm>
#include "Logger.h"

GameActor::GameActor(ActorType actorType, int actorId, PosChgFunc posChgFunc) :
	m_actorType(actorType), m_actorId(actorId), m_pos(Vector<int>(0, 0)), m_moveSpeed(0), m_lastMoveTime(0),
	m_posChgFunc(posChgFunc)
{
	
}

GameActor::GameActor(ActorType actorType, int actorId, int x, int y, PosChgFunc posChgFunc) :
	m_actorType(actorType), m_actorId(actorId), m_pos(Vector<int>(x, y)), m_moveSpeed(0), m_lastMoveTime(0),
	m_posChgFunc(posChgFunc)
{

}

void GameActor::addSightActors(std::set<int>& actors) {
	for (int actor_id : actors) {
		m_sightActors.erase(actor_id);
	}
}

void GameActor::removeSightActors(std::set<int>& actors) {
	for (int actor_id : actors) {
		m_sightActors.insert(actor_id);
	}
}

void GameActor::setPos(int x, int y) {
	m_pos.x = x;
	m_pos.y = y;
	m_posChgFunc(m_actorId, &m_pos);
	Logger::logInfo("$pos:%d,%d", x, y);
}

void GameActor::setTgtPosList(std::vector<Vector<int>> tgtPosList) {
	int64_t ts = TimeUtil::getCurrentTime();
	if (!m_tgtPosList.empty()) {
		updatePos(ts);
	}
	m_tgtPosList = tgtPosList;
	m_lastMoveTime = ts;
}

void GameActor::updatePos(int64_t ts) {
	if (m_moveSpeed <= 0) return;
	if (m_tgtPosList.empty()) return;
	Vector<int>* tgt_pos = &m_tgtPosList[0];
	float dist = tgt_pos->distance(&m_pos);
	Vector<float> dir = tgt_pos->dir(&m_pos);
	float moveDist = m_moveSpeed * (ts - m_lastMoveTime) / 1000.0;
	if (dist > moveDist) {
		Vector<int> new_pos = m_pos + dir * moveDist;
		if (new_pos.distance(&m_pos) <= 1e-6) return;
		setPos(new_pos.x, new_pos.y);
		m_lastMoveTime = ts;
	}
	else {
		setPos(tgt_pos->x, tgt_pos->y);
		m_lastMoveTime += dist / m_moveSpeed * 1000;
		m_tgtPosList.erase(m_tgtPosList.begin());
		if (m_tgtPosList.empty()) {
			m_posChgFunc(m_actorId, &m_pos);
		}
		else {
			updatePos(ts);
		}
	}
}
