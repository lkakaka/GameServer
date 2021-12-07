#include "SceneEntity.h"
#include "TimeUtil.h"
#include <algorithm>
#include "Logger.h"
#include "GameScene.h"
#include "proto.h"
#include "MyBuffer.h"
#include "../Common/ServerMacros.h"
#include "ServiceType.h"
#include "ServiceInfo.h"
#include "Network/ServiceCommEntityMgr.h"


SceneEntity::SceneEntity(SceneEntityType eType, int eid, void* gameScene, GridChgFunc gridChgFunc) :
	m_entityId(eid), m_entityType(eType), m_moveSpeed(0), m_gameScene(gameScene),
	m_pos(Position(0, 0)), m_grid(Grid(0, 0)), m_lastMoveTime(0),
	m_gridChgFunc(gridChgFunc) 
{
	
}

SceneEntity::SceneEntity(SceneEntityType eType, int eid, int x, int y, int moveSpeed, void* gameScene, GridChgFunc posChgFunc) :
	m_entityId(eid), m_entityType(eType), m_moveSpeed(moveSpeed), m_gameScene(gameScene),
	m_pos(Position(x, y)), m_grid(Grid(x / GRID_X_SIZE, y / GRID_Y_SIZE)), m_lastMoveTime(0),
	m_gridChgFunc(posChgFunc)
{

}

void SceneEntity::addSightEntities(std::set<int>& entityIds) {
	for (int eid : entityIds) {
		m_sightEntityIds.insert(eid);
	}
}

void SceneEntity::addSightEntity(int eid) {
	m_sightEntityIds.insert(eid);
}

void SceneEntity::removeSightEntities(std::set<int>& entityIds) {
	for (int eid : entityIds) {
		m_sightEntityIds.erase(eid);
	}
}

void SceneEntity::removeSightEntity(int eid) {
	m_sightEntityIds.erase(eid);
}

void SceneEntity::setPos(float x, float y, bool isTemp) {
	m_pos.x = x;
	m_pos.y = y;
	if (isTemp) return;

	GameScene* gameScene = m_gameScene != NULL ? (GameScene*)m_gameScene : NULL;
	if (m_grid.x != int(x / GRID_X_SIZE) || m_grid.y != int(y / GRID_Y_SIZE)) {
		m_grid.x = x / GRID_X_SIZE;
		m_grid.y = y / GRID_Y_SIZE;
		//m_gridChgFunc(m_actorId, &m_grid);
		if (gameScene != NULL) {
			gameScene->onEntityGridChg(m_entityId, &m_grid);
		}
	}

	if (gameScene != NULL) {
		SyncPos pos_msg;
		pos_msg.set_actor_id(m_entityId);
		pos_msg.set_pos_x(getX());
		pos_msg.set_pos_y(getY());
		broadcastMsgToSight(MSG_ID_SYNC_POS, &pos_msg);
		((GameScene*)m_gameScene)->onEntityPosChg(m_entityId, m_pos);
	}

	
	LOG_INFO("pos:%0.3f, %0.3f, gird_x:%d, gird_y:%d", x, y, m_grid.x, m_grid.y);
}

void SceneEntity::setTgtPosList(std::vector<Position>& tgtPosList) {
	int64_t ts = TimeUtil::nowMillSec();
	/*if (!m_tgtPosList.empty()) {
		updatePos(ts);
	}*/
	std::unique_lock<std::mutex> lock(m_tgtPosLock);
	m_srcTgtPosList = tgtPosList;
	m_lastMoveTime = ts;
}

void SceneEntity::updatePos(int64_t ts) {
	if (m_moveSpeed <= 0) return;
	{
		std::unique_lock<std::mutex> lock(m_tgtPosLock);
		if (!m_srcTgtPosList.empty()) {
			m_tgtPosList = m_srcTgtPosList;
			m_srcTgtPosList.clear();
		}
	}
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

std::set<int> SceneEntity::getSightEntityConndIds() {
	std::set<int> connIds;
	GameScene* gameScene = (GameScene*)m_gameScene;
	for (int eid : m_sightEntityIds) {
		SceneEntity* entity = gameScene->getEntity(eid);
		if (entity == NULL || !entity->isPlayer()) continue;
		PlayerEntity* player = (PlayerEntity*)entity;
		connIds.emplace(player->getConnId());
	}
	return connIds;
}

void SceneEntity::broadcastMsgToClient(std::set<int>& connIds, int msgId, google::protobuf::Message* msg) {
	std::string msgData;
	msg->SerializeToString(&msgData);
	broadcastMsgToClient(connIds, msgId, msgData.c_str(), msgData.length());
}

void SceneEntity::broadcastMsgToClient(std::set<int>& connIds, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // 是否是发给客户端的消息
	buffer.writeInt(msgId);
	buffer.writeInt(connIds.size()); // 连接数量
	for (int connId : connIds) {
		buffer.writeInt(connId);
	}
	buffer.writeByte(SEND_TYPE_TCP);
	buffer.writeString(msg, msgLen);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_GATEWAY, 0);
	SERVER_CENTER_COMM_ENTITY->sendToService(&addr, (char*)buffer.data(), buffer.size());
}

void SceneEntity::broadcastMsgToSight(int msgId, const char* msg, int msgLen) {
	if (m_sightEntityIds.size() == 0) return;
	std::set<int> connIds = getSightEntityConndIds();
	if (connIds.size() == 0) return;
	broadcastMsgToClient(connIds, msgId, msg, msgLen);
}

void SceneEntity::broadcastMsgToSight(int msgId, google::protobuf::Message* msg) {
	if (m_sightEntityIds.size() == 0) return;
	std::set<int> connIds = getSightEntityConndIds();
	if (connIds.size() == 0) return;
	std::string msgData;
	msg->SerializeToString(&msgData);
	broadcastMsgToClient(connIds, msgId, msgData.c_str(), msgData.length());
}
