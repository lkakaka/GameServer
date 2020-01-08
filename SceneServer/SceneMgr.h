#pragma once
#include <map>
#include "GameScene.h"
#include "SceneExport.h"

class SCENE_API SceneMgr
{
private:
	static SceneMgr* g_sceneMgr;
	int m_maxSceneUid;
	std::map<int, GameScene*> m_scenes;
	SceneMgr();
public:
	static SceneMgr* getSceneMgr();
	int allocSceneUid();
	GameScene* createScene(int sceneId);
	void destroyScene(int sceneUid);
};

