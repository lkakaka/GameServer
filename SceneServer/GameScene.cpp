#include "GameScene.h"
#include "Logger.h"


GameScene::GameScene(int sceneId, int sceneUid) : m_sceneId(sceneId), m_sceneUid(sceneUid)
{

}

void GameScene::onDestory()
{
	Logger::logInfo("$destory scene, sceneId:%d, sceneUid:%d", m_sceneId, m_sceneUid);
}