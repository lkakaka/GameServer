#include "SceneMgr.h"
#include "Logger.h"


SceneMgr* SceneMgr::g_sceneMgr = NULL;
SceneMgr* SceneMgr::getSceneMgr() {
	if (SceneMgr::g_sceneMgr != NULL) {
		return SceneMgr::g_sceneMgr;
	}
	SceneMgr::g_sceneMgr = new SceneMgr();
	return SceneMgr::g_sceneMgr;
}

SceneMgr::SceneMgr(): m_maxSceneUid(0)
{}


int SceneMgr::allocSceneUid() {
	return m_maxSceneUid++;
}

GameScene* SceneMgr::createScene(int sceneId, void* scriptObj)
{
	int sceneUid = allocSceneUid();
	if (m_scenes.find(sceneUid) != m_scenes.end()) {
		Logger::logError("$alloc scene failed, scene uid(%d) exist", sceneUid);
		return NULL;
	}
	GameScene* gameScene = new GameScene(sceneId, sceneUid, scriptObj);
	m_scenes.emplace(std::make_pair(sceneUid, gameScene));
	return gameScene;
}

void SceneMgr::destroyScene(int sceneUid) {
	auto iter = m_scenes.find(sceneUid);
	if (iter == m_scenes.end()) {
		Logger::logError("$destory scene failed, scene uid(%d) not exist", sceneUid);
		return;
	}
	m_scenes.erase(sceneUid);
	GameScene* gameScene = (GameScene*)iter->second;
	gameScene->onDestory();
	delete gameScene;
}