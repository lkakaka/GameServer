#pragma once
//#include "GameActor.h"
#include <map>
#include "GamePlayer.h"
#include "GameNpc.h"
#include "AOIMgr.h"

class GameScene
{
private:
	int m_sceneId;
	int m_sceneUid;
	std::map<int, GameActor*> m_actors;
	void* m_scriptObj;
	int m_maxActorId;

	void onPlayerEnter(GamePlayer* gamePlayer, std::vector<int>& neighbours);
	void onNpcEnter(GameNpc* gameNpc, std::vector<int>& neighbours);
public:
	AOIMgr m_AOIMgr;

	inline int getSceneUid() { return m_sceneUid; }
	inline int getSceneId() { return m_sceneId; }
	GameScene(int sceneId, int sceneUid, void* scriptObj);

	void onDestory();
	GamePlayer* createPlayer(int connId, int roleId, const char* name, int x, int y);
	void onActorEnter(int actorId);
	void onActorLeave(GameActor* gameActor);
	void onActorMove(GameActor* gameActor);
	GameActor* getActor(int actorId);
	void removeActor(int actorId);
	void onCreate();

	void setActorPos(int actorId, int x, int y);
};

