#include "GameScene.h"
#include "Logger.h"


GameScene::GameScene(int sceneId, int sceneUid, void* scriptObj) : m_maxActorId(0), m_sceneId(sceneId), m_sceneUid(sceneUid),
m_scriptObj(scriptObj)
{

}

void GameScene::onDestory()
{
	Logger::logInfo("$destory scene, sceneId:%d, sceneUid:%d", m_sceneId, m_sceneUid);
}

GamePlayer* GameScene::createPlayer(int connId, int roleId, const char* name)
{
	int actorId = m_maxActorId++;
	GamePlayer* gamePlayer = new GamePlayer(connId, actorId, roleId, name);
	m_actors.emplace(std::make_pair(actorId, gamePlayer));
	Logger::logInfo("$create player, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
	return gamePlayer;
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
	delete gameActor;
	m_actors.erase(iter);
	Logger::logInfo("$remove actor, sceneId:%d, sceneUid:%d, actorId:%d", m_sceneId, m_sceneUid, actorId);
}