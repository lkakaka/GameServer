#include "SceneMgr.h"
#include "Logger.h"
#include "proto.h"


SceneMgr* SceneMgr::g_sceneMgr = NULL;
SceneMgr* SceneMgr::getSceneMgr() {
	if (SceneMgr::g_sceneMgr != NULL) {
		return SceneMgr::g_sceneMgr;
	}
	SceneMgr::g_sceneMgr = new SceneMgr();
	return SceneMgr::g_sceneMgr;
}

SceneMgr::SceneMgr(): m_maxSceneUid(1)
{
	m_serviceId = Config::getSingleton()->getConfigInt("service_id");
}


int SceneMgr::allocSceneUid() {
	return (m_serviceId << 20) + m_maxSceneUid++;
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
	gameScene->onCreate();
	Logger::logInfo("$create scene, scene_uid:%d, sceneId:%d", sceneUid, sceneId);
	return gameScene;
}

GameScene* SceneMgr::getScene(int sceneUid) {
	auto iter = m_scenes.find(sceneUid);
	if (iter == m_scenes.end()) {
		Logger::logError("$get scene failed, scene uid(%d) not exist", sceneUid);
		return NULL;
	}
	return iter->second;
}

void SceneMgr::destroyScene(int sceneUid) {
	auto iter = m_scenes.find(sceneUid);
	if (iter == m_scenes.end()) {
		Logger::logError("$destory scene failed, scene uid(%d) not exist", sceneUid);
		return;
	}
	GameScene* gameScene = (GameScene*)iter->second;
	gameScene->onDestory();
	Logger::logInfo("$destroy scene, scene_uid:%d, sceneId:%d", sceneUid, gameScene->getSceneId());
	delete gameScene;
	m_scenes.erase(sceneUid);
}

void SceneMgr::addPlayer(int conn_id, int scene_uid) {
	m_player_scene.emplace(conn_id, scene_uid);
}

void SceneMgr::removePlayer(int conn_id) {
	m_player_scene.erase(conn_id);
}

bool SceneMgr::handleClientMsg(int connId, int msgId, char* data, int dataLen) {
	if (msgId != MSG_ID_MOVE_TO) return false;
	auto iter = m_player_scene.find(connId);
	if (iter == m_player_scene.end()) {
		return false;
	}
	GameScene* scene = getScene(iter->second);
	if (scene == NULL) {
		return false;
	}
	return scene->onRecvClientMsg(connId, msgId, data, dataLen);
}