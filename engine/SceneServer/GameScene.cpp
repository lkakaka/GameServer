#include "GameScene.h"
#include "Logger.h"
//#include "PyScene.h"
//#include "../Common/PyCommon.h"
#include "SceneMgr.h"
#include "TimeUtil.h"


GameScene::GameScene(int sceneId, int sceneUid, void* scriptObj) : m_maxActorId(0), m_sceneId(sceneId), m_sceneUid(sceneUid),
m_scriptObj(scriptObj), m_detour(new SceneDetourMgr()), m_scriptObjId(0), m_luaRef(0), m_callScriptFunc(NULL)
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
}

void GameScene::_syncThreadFunc() {
	while (true) {
		int64_t ts = TimeUtil::getCurrentTime();
		for (auto iter = m_actors.begin(); iter != m_actors.end(); iter++) {
			GameActor* gameActor = iter->second;
			if (gameActor == NULL || !gameActor->isMoving()) continue;
			gameActor->updatePos(ts);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void GameScene::onDestory()
{
	Logger::logInfo("$destory scene, sceneId:%d, sceneUid:%d", m_sceneId, m_sceneUid);
}

GamePlayer* GameScene::createPlayer(int connId, int roleId, const char* name, int x, int y)
{
	int actorId = m_maxActorId++;
	GamePlayer* gamePlayer = new GamePlayer(connId, actorId, roleId, name, x, y, this, std::bind(&GameScene::onActorGridChg, this, std::placeholders::_1, std::placeholders::_2));
	m_actors.emplace(std::make_pair(actorId, gamePlayer));
	m_players.emplace(std::make_pair(connId, gamePlayer));
	SceneMgr::getSceneMgr()->addPlayer(connId, m_sceneUid);
	Logger::logInfo("$create player, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
	return gamePlayer;
}

void GameScene::onActorEnter(int actorId) {
	GameActor* actor = getActor(actorId);
	if (actor == NULL) {
		Logger::logError("$on actor enter error, not found actorId:%d", actorId);
		return;
	}

	std::set<int> neighbours;
	m_AOIMgr.addNode(actor->getActorId(), actor->getGridX(), actor->getGridY(), neighbours);

	if (!neighbours.empty()) {
		onEnterSight(actor, neighbours);

		/*auto py_state = PyGILState_Ensure();
		PyObject* arg = PyTuple_New(2);
		PyObject* actors = PyTuple_New(neighbours.size());
		int i = 0;
		for (int actorId : neighbours) {
			PyTuple_SetItem(actors, i++, PyLong_FromLong(actorId));
		}
		PyTuple_SetItem(arg, 0, PyLong_FromLong(actor->getActorId()));
		PyTuple_SetItem(arg, 1, actors);
		callPyObjFunc((PyObject*)m_scriptObj, "after_actor_enter", arg);
		PyGILState_Release(py_state);*/

		if (m_callScriptFunc != NULL) {
			m_callScriptFunc(this, SceneScriptEvent::AFTER_ACTOR_ENTER, actor->getActorId(), neighbours);
		}
	}

	if (actor->getActorType() == ActorType::PLYAER) {
		onPlayerEnter((GamePlayer*)actor, neighbours);
	} else if(actor->getActorType() == ActorType::NPC) {
		onNpcEnter((GameNpc*)actor, neighbours);
	} else {
		Logger::logError("$on actor enter error, unkown actor type:%d", actor->getActorType());
	}
}

void GameScene::onPlayerEnter(GamePlayer* gamePlayer, std::set<int>& neighbours) {
	
}

void GameScene::onNpcEnter(GameNpc* gameNpc, std::set<int>& neighbours) {
	
}

void GameScene::onEnterSight(GameActor* actor, std::set<int>& enterIds) {
	actor->addSightActors(enterIds);
	for (int actorId : enterIds) {
		GameActor* neiActor = getActor(actorId);
		if (neiActor == NULL) continue;
		neiActor->addSightActor(actorId);
	}
}

void GameScene::onLeaveSight(GameActor* actor, std::set<int>& leaveIds) {
	actor->removeSightActors(leaveIds);
	for (int actorId : leaveIds) {
		GameActor* neiActor = getActor(actorId);
		if (neiActor == NULL) continue;
		neiActor->removeSightActor(actorId);
	}
}

void GameScene::onActorLeave(GameActor* gameActor) {
	std::set<int> neighbours;
	m_AOIMgr.removeNode(gameActor->getActorId(), neighbours);

	if (!neighbours.empty()) {
		onLeaveSight(gameActor, neighbours);

		/*auto py_state = PyGILState_Ensure();
		PyObject* arg = PyTuple_New(2);
		PyObject* actors = PyTuple_New(neighbours.size());
		int i = 0;
		for (int actorId : neighbours) {
			PyTuple_SetItem(actors, i++, PyLong_FromLong(actorId));
		}
		PyTuple_SetItem(arg, 0, PyLong_FromLong(gameActor->getActorId()));
		PyTuple_SetItem(arg, 1, actors);
		callPyObjFunc((PyObject*)m_scriptObj, "after_actor_leave", arg);
		PyGILState_Release(py_state);*/

		if (m_callScriptFunc != NULL) {
			m_callScriptFunc(this, SceneScriptEvent::AFTER_ACTOR_LEAVE, gameActor->getActorId(), neighbours);
		}
	}
}

void GameScene::onActorMove(GameActor* gameActor) {
	std::set<int> enterIds;
	std::set<int> leaveIds;
	m_AOIMgr.moveNode(gameActor->getActorId(), gameActor->getGridX(), gameActor->getGridY(), leaveIds, enterIds);
	if (enterIds.empty() && leaveIds.empty()) return;

	onEnterSight(gameActor, enterIds);
	onLeaveSight(gameActor, leaveIds);

	/*auto py_state = PyGILState_Ensure();
	PyObject* arg = PyTuple_New(3);
	PyObject* enterTuple = PyTuple_New(enterIds.size());
	PyObject* leaveTuple = PyTuple_New(leaveIds.size());
	int i = 0;
	for (int actorId : enterIds) {
		PyTuple_SetItem(enterTuple, i++, PyLong_FromLong(actorId));
	}

	i = 0;
	for (int actorId : leaveIds) {
		PyTuple_SetItem(leaveTuple, i++, PyLong_FromLong(actorId));
	}

	PyTuple_SetItem(arg, 0, PyLong_FromLong(gameActor->getActorId()));
	PyTuple_SetItem(arg, 1, enterTuple);
	PyTuple_SetItem(arg, 2, leaveTuple);
	callPyObjFunc((PyObject*)m_scriptObj, "after_actor_move", arg);
	PyGILState_Release(py_state);*/

	if (m_callScriptFunc != NULL) {
		m_callScriptFunc(this, SceneScriptEvent::AFTER_ACTOR_MOVE, gameActor->getActorId(), enterIds, leaveIds);
	}
}

GameActor* GameScene::getActor(int actorId) {
	auto iter = m_actors.find(actorId);
	if (iter == m_actors.end()) {
		Logger::logError("$get actor error, actor not found, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
		return NULL;
	}

	return iter->second;
}

GamePlayer* GameScene::getPlayer(int connId) {
	auto iter = m_players.find(connId);
	if (iter == m_players.end()) {
		Logger::logError("$get player error, player not found, sceneId:%d, sceneUid:%d, connId:%d", m_sceneId, m_sceneUid, connId);
		return NULL;
	}

	return iter->second;
}

void GameScene::removeActor(int actorId) {
	auto iter = m_actors.find(actorId);
	if (iter == m_actors.end()) {
		Logger::logError("$remove actor error, actor not found, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
		return;
	}

	GameActor* gameActor = iter->second;
	onActorLeave(gameActor);

	if (gameActor->getActorType() == ActorType::PLYAER) {
		int connId = ((GamePlayer*)gameActor)->getConnId();
		SceneMgr::getSceneMgr()->removePlayer(connId);
		m_players.erase(connId);
	}

	delete gameActor;
	m_actors.erase(iter);
	Logger::logInfo("$remove actor, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
}

void GameScene::onActorGridChg(int actorId, Grid* grid) {
	GameActor* actor = getActor(actorId);
	if (actor == NULL) {
		Logger::logError("move actor error, actor not found, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
		return;
	}

	onActorMove(actor);
	Logger::logDebug("$actor grid chg!!!");
}

void GameScene::onActorPosChg(int actorId, Position& pos) {
	
}

bool GameScene::onRecvClientMsg(int connId, int msgId, char* data, int dataLen) {
	GamePlayer* player = getPlayer(connId);
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
