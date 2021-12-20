#include "GameScene.h"
#include "Logger.h"
//#include "PyScene.h"
//#include "../Common/PyCommon.h"
#include "SceneMgr.h"
#include "TimeUtil.h"
#include "Timer.h"
#include "AsioService.h"


GameScene::GameScene(int sceneId, int sceneUid) : m_sceneId(sceneId), m_sceneUid(sceneUid), m_maxEntityId(0),
 m_detour(new SceneDetourMgr())
{
}

void GameScene::onCreate()
{
	/*std::vector<int> neighbours1;
	m_AOIMgr.addNode(1, 20, 20, neighbours1);
	printf("addr=%p.\n", &neighbours1);
	neighbours1.clear();
	m_AOIMgr.addNode(2, 30, 10, neighbours1);
	neighbours1.clear();
	m_AOIMgr.addNode(3, 10, 30, neighbours1);
	m_AOIMgr.dump();

	std::vector<int> enters, leaves;
	m_AOIMgr.moveNode(2, 15, 25, enters, leaves);
	m_AOIMgr.dump();*/

	/*m_AOIMgr.removeNode(2);
	m_AOIMgr.dump();*/
	m_syncThread.reset(new std::thread(std::bind(&GameScene::_syncThreadFunc, this)));
	/*TimerMgr::getSingleton()->addTimer(33, 33, -1, [this](int timerId) {
		m_logicTaskMgr.runTask();
	});*/
}

void GameScene::_syncThreadFunc() {
	while (true) {
		int64_t ts = TimeUtil::nowMillSec();
		for (auto iter = m_entities.begin(); iter != m_entities.end(); iter++) {
			SceneEntity* entity = iter->second;
			if (entity == NULL) continue;
			entity->updatePos(ts);
		}
		//m_syncTaskMgr.runTask();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void GameScene::onDestory()
{
	m_syncThread.reset();
	LOG_INFO("destory scene, sceneId:%d, sceneUid:%d", m_sceneId, m_sceneUid);
}

PlayerEntity* GameScene::createPlayer(int connId, int64_t roleId, const char* name, int x, int y, int moveSpeed)
{
	int eid = m_maxEntityId++;
	PlayerEntity* player = new PlayerEntity(connId, eid, roleId, name, x, y, moveSpeed, this, std::bind(&GameScene::onEntityGridChg, this, std::placeholders::_1, std::placeholders::_2));
	m_entities.emplace(std::make_pair(eid, player));
	m_players.emplace(std::make_pair(connId, player));
	SceneMgr::getSceneMgr()->addPlayer(connId, m_sceneUid);
	LOG_INFO("create player, sceneId:%d, sceneUid:%d, eid:%d", m_sceneId, m_sceneUid, eid);
	return player;
}

NpcEntity* GameScene::createNpc(int npcId, int x, int y, int moveSpeed)
{
	int eid = m_maxEntityId++;
	NpcEntity* npc = new NpcEntity(eid, npcId, x, y, moveSpeed, this, std::bind(&GameScene::onEntityGridChg, this, std::placeholders::_1, std::placeholders::_2));
	m_entities.emplace(std::make_pair(eid, npc));
	LOG_INFO("create npc, sceneId:%d, sceneUid:%d, eid:%d", m_sceneId, m_sceneUid, eid);
	return npc;
}

void GameScene::onEntityEnter(int eid) {
	SceneEntity* entity = getEntity(eid);
	if (entity == NULL) {
		LOG_ERROR("on entity enter error, not found eid:%d", eid);
		return;
	}

	std::set<int> neighbours;
	m_AOIMgr.addNode(entity->getEntityId(), entity->getGridX(), entity->getGridY(), neighbours);

	if (!neighbours.empty()) {
		onEnterSight(entity, neighbours);

		CallScripFunc func = getCallScriptFunc();
		if (func != NULL) {
			func(this, SceneScriptEvent::AFTER_ENTITY_ENTER, entity->getEntityId(), neighbours);
		}
	}

	if (entity->isPlayer()) {
		onPlayerEnter((PlayerEntity*)entity, neighbours);
	} else if(entity->isNpc()) {
		onNpcEnter((NpcEntity*)entity, neighbours);
	} else {
		LOG_ERROR("on entity enter error, unkown entity type:%d", entity->getEntityType());
	}
}

void GameScene::onPlayerEnter(PlayerEntity* player, std::set<int>& neighbours) {
	
}

void GameScene::onNpcEnter(NpcEntity* npc, std::set<int>& neighbours) {
	
}

void GameScene::onEnterSight(SceneEntity* entity, std::set<int>& enterIds) {
	entity->addSightEntities(enterIds);
	for (int eid : enterIds) {
		SceneEntity* neiEntity = getEntity(eid);
		if (neiEntity == NULL) {
			LOG_DEBUG("enter sight entity not found, eid:%d", eid);
			continue;
		}
		neiEntity->addSightEntity(entity->getEntityId());
	}
}

void GameScene::onLeaveSight(SceneEntity* entity, std::set<int>& leaveIds) {
	entity->removeSightEntities(leaveIds);
	for (int eid : leaveIds) {
		SceneEntity* neiEntity = getEntity(eid);
		if (neiEntity == NULL) {
			LOG_DEBUG("leave sight entity not found, eid:%d", eid);
			continue;
		}
		neiEntity->removeSightEntity(entity->getEntityId());
	}
}

void GameScene::onEntityLeave(SceneEntity* entity) {
	std::set<int> neighbours;
	m_AOIMgr.removeNode(entity->getEntityId(), neighbours);

	if (!neighbours.empty()) {
		onLeaveSight(entity, neighbours);

		CallScripFunc func = getCallScriptFunc();
		if (func != NULL) {
			func(this, SceneScriptEvent::AFTER_ENTITY_LEAVE, entity->getEntityId(), neighbours);
		}
	}
}

void GameScene::onEntityMove(SceneEntity* entity) {
	std::set<int> enterIds;
	std::set<int> leaveIds;
	m_AOIMgr.moveNode(entity->getEntityId(), entity->getGridX(), entity->getGridY(), leaveIds, enterIds);
	if (enterIds.empty() && leaveIds.empty()) return;

	onEnterSight(entity, enterIds);
	onLeaveSight(entity, leaveIds);

	runLogicTask([this, entity, enterIds, leaveIds]() {
		CallScripFunc func = getCallScriptFunc();
		if (func != NULL) {
			func(this, SceneScriptEvent::AFTER_ENTITY_MOVE, entity->getEntityId(), enterIds, leaveIds);
		}
	});
}

SceneEntity* GameScene::getEntity(int eid) {
	auto iter = m_entities.find(eid);
	if (iter == m_entities.end()) {
		LOG_ERROR("get entity error, entity not found, sceneId:%d, sceneUid:%d, eid:%d", m_sceneId, m_sceneUid, eid);
		return NULL;
	}

	return iter->second;
}

PlayerEntity* GameScene::getPlayer(int connId) {
	auto iter = m_players.find(connId);
	if (iter == m_players.end()) {
		LOG_ERROR("get player error, player not found, sceneId:%d, sceneUid:%d, connId:%d", m_sceneId, m_sceneUid, connId);
		return NULL;
	}

	return iter->second;
}

void GameScene::changePlayerConnId(int oldConnId, int connId) {
	auto iter = m_players.find(oldConnId);
	if (iter == m_players.end()) {
		return;
	}
	PlayerEntity* player = iter->second;
	m_players.erase(iter);
	m_players.emplace(std::make_pair(connId, player));
	SceneMgr::getSceneMgr()->changePlayerConnId(oldConnId, connId);
}

void GameScene::removeEntity(int eid) {
	auto iter = m_entities.find(eid);
	if (iter == m_entities.end()) {
		LOG_ERROR("remove entity error, entity not found, sceneId:%d, sceneUid:%d, eid:%d", m_sceneId, m_sceneUid, eid);
		return;
	}

	SceneEntity* entity = iter->second;
	onEntityLeave(entity);

	if (entity->isPlayer()) {
		int connId = ((PlayerEntity*)entity)->getConnId();
		SceneMgr::getSceneMgr()->removePlayer(connId);
		m_players.erase(connId);
	}

	delete entity;
	m_entities.erase(iter);
	LOG_INFO("remove entity, sceneId:%d, sceneUid:%d, eid:%d", m_sceneId, m_sceneUid, eid);
}

void GameScene::onEntityGridChg(int eid, Grid* grid) {
	SceneEntity* entity = getEntity(eid);
	if (entity == NULL) {
		LOG_ERROR("move entity error, entity not found, sceneId:%d, sceneUid:%d, eid:%d", m_sceneId, m_sceneUid, eid);
		return;
	}

	onEntityMove(entity);
	LOG_DEBUG("entity grid chg!!!");
}

void GameScene::onEntityPosChg(int eid, Position& pos) {
	
}

bool GameScene::onRecvClientMsg(int connId, int msgId, char* data, int dataLen) {
	PlayerEntity* player = getPlayer(connId);
	if (player == NULL) {
		return false;
	}
	return player->onRecvClientMsg(msgId, data, dataLen);
}

bool GameScene::loadNavMesh(const char* meshFileName) {
	return m_detour->initNavMesh(meshFileName);
}

void GameScene::findPath(float* sPos, float* ePos, std::vector<float>* path) {
	m_detour->findPath(sPos, ePos, path);
}

void GameScene::runLogicTask(std::function<void()> task) {
	MAIN_IO_SERVICE_PTR->async_run_task(task);
}

//void GameScene::addSyncTask(std::function<void()> task) {
//	m_syncTaskMgr->addTask(task);
//}
