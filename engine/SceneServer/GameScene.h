#pragma once
//#include "GameActor.h"
#include <map>
#include "GamePlayer.h"
#include "GameNpc.h"
#include "AOIMgr.h"
#include <thread>
#include <mutex>
#include "DetourMgr.h"
#include "Vector.h"
#include "IScriptRegister.h"
#include "TaskMgr.h"

enum SceneScriptEvent {
	AFTER_ACTOR_ENTER = 1,
	AFTER_ACTOR_LEAVE = 2,
	AFTER_ACTOR_MOVE = 3,
};

class GameScene : public IScriptRegister
{
private:
	int m_sceneId;
	int m_sceneUid;
	std::map<int, GameActor*> m_actors;
	std::map<int, GamePlayer*> m_players;
	int m_maxActorId;
	std::shared_ptr<std::thread> m_syncThread;

	std::shared_ptr<SceneDetourMgr> m_detour;

	//std::shared_ptr<TaskMgr> m_syncTaskMgr;

	void onPlayerEnter(GamePlayer* gamePlayer, std::set<int>& neighbours);
	void onNpcEnter(GameNpc* gameNpc, std::set<int>& neighbours);

	void onEnterSight(GameActor* actor, std::set<int>& enterIds);
	void onLeaveSight(GameActor* actor, std::set<int>& leaveIds);

	void _syncThreadFunc();

public:
	AOIMgr m_AOIMgr;

	inline int getSceneUid() { return m_sceneUid; }
	inline int getSceneId() { return m_sceneId; }
	GameScene(int sceneId, int sceneUid);

	void onDestory();
	GamePlayer* createPlayer(int connId, int roleId, const char* name, int x, int y, int moveSpeed);
	GameNpc* createNpc(int npcId, int x, int y, int moveSpeed);
	void onActorEnter(int actorId);
	void onActorLeave(GameActor* gameActor);
	void onActorMove(GameActor* gameActor);
	GameActor* getActor(int actorId);
	GamePlayer* getPlayer(int connId);
	void removeActor(int actorId);
	void onCreate();

	void onActorGridChg(int actorId, Grid* pos);
	void onActorPosChg(int actorId, Position& pos);

	bool onRecvClientMsg(int connId, int msgId, char* data, int dataLen);

	bool loadNavMesh(const char* meshFileName);
	void findPath(float *sPos, float* ePos, std::vector<float>* path);

	void runLogicTask(std::function<void()> func);
	//void addSyncTask(std::function<void()> func);
};


