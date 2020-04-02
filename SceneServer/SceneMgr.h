#pragma once
#include <map>
#include "GameScene.h"
#include "../Common/ServerExports.h"

class SERVER_EXPORT_API SceneMgr
{
private:
	static SceneMgr* g_sceneMgr;
	int m_maxSceneUid;
	std::map<int, GameScene*> m_scenes;
	SceneMgr();
public:
	static SceneMgr* getSceneMgr();
	int allocSceneUid();
	GameScene* createScene(int sceneId, void* scriptObj);
	GameScene* getScene(int sceneUid);
	void destroyScene(int sceneUid);
};

