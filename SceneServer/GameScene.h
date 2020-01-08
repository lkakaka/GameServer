#pragma once
#include "GameActor.h"
#include <map>

class GameScene
{
private:
	int m_sceneId;
	int m_sceneUid;
	std::map<int, GameActor*> m_actors;
public:
	GameScene(int sceneId, int sceneUid);

	void onDestory();
};

