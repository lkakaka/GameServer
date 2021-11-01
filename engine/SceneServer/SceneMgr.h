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
	std::map<int, int> m_player_scene;
	int m_serviceId;
	SceneMgr();
public:
	static SceneMgr* getSceneMgr();
	int allocSceneUid();
	GameScene* createScene(int sceneId);
	GameScene* getScene(int sceneUid);
	void destroyScene(int sceneUid);

	void addPlayer(int conn_id, int scene_uid);
	void changePlayerConnId(int oldConnId, int connId);
	void removePlayer(int conn_id);

	bool handleClientMsg(int connId, int msgId, char* data, int dataLen);
};

