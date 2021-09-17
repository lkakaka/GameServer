#include "GameActor.h"
#include "TimeUtil.h"
#include <algorithm>
#include "Logger.h"
#include "GameScene.h"
#include "proto.h"


GameActor::GameActor(ActorType actorType, int actorId, void* gameScene, GridChgFunc gridChgFunc) :
	m_actorType(actorType), m_actorId(actorId), m_pos(Position(0, 0)), m_grid(Grid(0, 0)), m_moveSpeed(0), m_lastMoveTime(0),
	m_gridChgFunc(gridChgFunc), m_gameScene(gameScene)
{
	
}

GameActor::GameActor(ActorType actorType, int actorId, int x, int y, void* gameScene, GridChgFunc posChgFunc) :
	m_actorType(actorType), m_actorId(actorId), m_pos(Position(x, y)), m_grid(Grid(x / GRID_X_SIZE, y / GRID_Y_SIZE)), m_moveSpeed(0), m_lastMoveTime(0),
	m_gridChgFunc(posChgFunc), m_gameScene(gameScene)
{

}

void GameActor::addSightActors(std::set<int>& actors) {
	for (int actor_id : actors) {
		m_sightActors.insert(actor_id);
	}
}

void GameActor::addSightActor(int actorId) {
	m_sightActors.insert(actorId);
}

void GameActor::removeSightActors(std::set<int>& actors) {
	for (int actor_id : actors) {
		m_sightActors.erase(actor_id);
	}
}

void GameActor::removeSightActor(int actorId) {
	m_sightActors.erase(actorId);
}

void GameActor::setPos(float x, float y, bool isTemp) {
	m_pos.x = x;
	m_pos.y = y;
	if (isTemp) return;

	GameScene* gameScene = m_gameScene != NULL ? (GameScene*)m_gameScene : NULL;
	if (m_grid.x != int(x / GRID_X_SIZE) || m_grid.y != int(y / GRID_Y_SIZE)) {
		m_grid.x = x / GRID_X_SIZE;
		m_grid.y = y / GRID_Y_SIZE;
		//m_gridChgFunc(m_actorId, &m_grid);
		if (gameScene != NULL) {
			gameScene->onActorGridChg(m_actorId, &m_grid);
		}
	}

	if (gameScene != NULL) {
		SyncPos pos_msg;
		pos_msg.set_actor_id(m_actorId);
		pos_msg.set_pos_x(getX());
		pos_msg.set_pos_y(getY());
		for (int actor_id : m_sightActors) {
			GameActor* gameActor = gameScene->getActor(actor_id);
			if (gameActor == NULL || !gameActor->isPlayer()) continue;
			((GamePlayer*)gameActor)->sendToClient(MSG_ID_SYNC_POS, &pos_msg);
		}

		((GameScene*)m_gameScene)->onActorPosChg(m_actorId, m_pos);
	}

	
	LOG_INFO("pos:%0.3f, %0.3f, gird_x:%d, gird_y:%d", x, y, m_grid.x, m_grid.y);
}

void GameActor::setTgtPosList(std::vector<Position> tgtPosList) {
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
	Position tgt_pos = m_tgtPosList[0];
	Vector2<float> path = tgt_pos - m_pos;
	float dist = path.length();
	Vector2<float> dir = path.normalize();
	//Vector2<float> dir = tgt_pos->dir(&m_pos);
	float moveDist = m_moveSpeed * (ts - m_lastMoveTime) / 1000.0;
	if (dist > moveDist) {
		Position tmpPos = m_pos + dir * moveDist;
		if ((tmpPos - m_pos).length_2() <= 1e-6) return;
		setPos(tmpPos.x, tmpPos.y);
		m_lastMoveTime = ts;
	}
	else {
		m_lastMoveTime += dist / m_moveSpeed * 1000;
		m_tgtPosList.erase(m_tgtPosList.begin());
		if (!m_tgtPosList.empty()) {
			setPos(tgt_pos.x, tgt_pos.y, true);
			updatePos(ts);
		}
		else {
			setPos(tgt_pos.x, tgt_pos.y);
		}
	}
}
