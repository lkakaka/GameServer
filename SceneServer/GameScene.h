#pragma once
#include "GameActor.h"
#include <map>
#include "GamePlayer.h"

class GameScene
{
private:
	int m_sceneId;
	int m_sceneUid;
	std::map<int, GameActor*> m_actors;
	void* m_scriptObj;
	int m_maxActorId;
public:
	inline int getSceneUid() { return m_sceneUid; }
	inline int getSceneId() { return m_sceneId; }
	GameScene(int sceneId, int sceneUid, void* scriptObj);

	void onDestory();
	GamePlayer* createPlayer(int connId, int roleId, const char* name);
	void removeActor(int actorId);
};

