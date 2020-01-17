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
	return gamePlayer;
}

void GameScene::removeActor(int actorId) {
	auto iter = m_actors.find(actorId);
	if (iter == m_actors.end()) {
		return;
	}
	GameActor* gameActor = iter->second;
	delete gameActor;
	m_actors.erase(iter);
}