#include "GameScene.h"
#include "Logger.h"
#include "PyScene.h"


GameScene::GameScene(int sceneId, int sceneUid, void* scriptObj) : m_maxActorId(0), m_sceneId(sceneId), m_sceneUid(sceneUid),
m_scriptObj(scriptObj)
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
}

void GameScene::onDestory()
{
	Logger::logInfo("$destory scene, sceneId:%d, sceneUid:%d", m_sceneId, m_sceneUid);
}

GamePlayer* GameScene::createPlayer(int connId, int roleId, const char* name, int x, int y)
{
	int actorId = m_maxActorId++;
	GamePlayer* gamePlayer = new GamePlayer(connId, actorId, roleId, name, x, y);
	m_actors.emplace(std::make_pair(actorId, gamePlayer));
	Logger::logInfo("$create player, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
	return gamePlayer;
}

void GameScene::onActorEnter(int actorId) {
	GameActor* actor = getActor(actorId);
	if (actor == NULL) {
		Logger::logError("$on actor enter error, not found actorId:%d", actorId);
		return;
	}

	std::vector<int> neighbours;
	m_AOIMgr.addNode(actor->getActorId(), actor->getX(), actor->getY(), neighbours);

	if (!neighbours.empty()) {
		auto py_state = PyGILState_Ensure();
		PyObject* arg = PyTuple_New(2);
		PyObject* actors = PyTuple_New(neighbours.size());
		int i = 0;
		for (int actorId : neighbours) {
			PyTuple_SetItem(actors, i++, PyLong_FromLong(actorId));
		}
		PyTuple_SetItem(arg, 0, PyLong_FromLong(actor->getActorId()));
		PyTuple_SetItem(arg, 1, actors);
		callPyFunction((PyObject*)m_scriptObj, "on_actor_enter", arg);
		PyGILState_Release(py_state);
	}

	if (actor->getActorType() == ActorType::PLYAER) {
		onPlayerEnter((GamePlayer*)actor, neighbours);
	} else if(actor->getActorType() == ActorType::NPC) {
		onNpcEnter((GameNpc*)actor, neighbours);
	} else {
		Logger::logError("$on actor enter error, unkown actor type:%d", actor->getActorType());
	}
}

void GameScene::onPlayerEnter(GamePlayer* gamePlayer, std::vector<int>& neighbours) {
	
}

void GameScene::onNpcEnter(GameNpc* gameNpc, std::vector<int>& neighbours) {
	
}

void GameScene::onActorLeave(GameActor* gameActor) {
	std::vector<int> neighbours;
	m_AOIMgr.removeNode(gameActor->getActorId(), neighbours);

	if (!neighbours.empty()) {
		auto py_state = PyGILState_Ensure();
		PyObject* arg = PyTuple_New(2);
		PyObject* actors = PyTuple_New(neighbours.size());
		int i = 0;
		for (int actorId : neighbours) {
			PyTuple_SetItem(actors, i++, PyLong_FromLong(actorId));
		}
		PyTuple_SetItem(arg, 0, PyLong_FromLong(gameActor->getActorId()));
		PyTuple_SetItem(arg, 1, actors);
		callPyFunction((PyObject*)m_scriptObj, "on_actor_leave", arg);
		PyGILState_Release(py_state);
	}
}

void GameScene::onActorMove(GameActor* gameActor) {
	std::vector<int> enterIds;
	std::vector<int> leaveIds;
	m_AOIMgr.moveNode(gameActor->getActorId(), gameActor->getX(), gameActor->getY(), leaveIds, enterIds);
	if (enterIds.empty() && leaveIds.empty()) return;

	auto py_state = PyGILState_Ensure();
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
	callPyFunction((PyObject*)m_scriptObj, "on_actor_move", arg);
	PyGILState_Release(py_state);
}

GameActor* GameScene::getActor(int actorId) {
	auto iter = m_actors.find(actorId);
	if (iter == m_actors.end()) {
		Logger::logError("$get actor error, actor not found, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
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

	delete gameActor;
	m_actors.erase(iter);
	Logger::logInfo("$remove actor, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
}

void GameScene::setActorPos(int actorId, int x, int y) {
	GameActor* actor = getActor(actorId);
	if (actor == NULL) {
		Logger::logError("move actor error, actor not found, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
		return;
	}

	actor->setPos(x, y);
	onActorMove(actor);
}

