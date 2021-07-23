#pragma once
//#include "GameActor.h"
#include <map>
#include "GamePlayer.h"
#include "GameNpc.h"
#include "AOIMgr.h"
#include <thread>
#include "DetourMgr.h"
#include "Vector.h"

typedef void (*CallSceneScripFunc)(void* gameScene, int scriptEvent, ...);

enum SceneScriptEvent {
	AFTER_ACTOR_ENTER = 1,
	AFTER_ACTOR_LEAVE = 2,
	AFTER_ACTOR_MOVE = 3,
};

class GameScene
{
private:
	int m_sceneId;
	int m_sceneUid;
	std::map<int, GameActor*> m_actors;
	std::map<int, GamePlayer*> m_players;
	void* m_scriptObj;
	int m_maxActorId;
	std::shared_ptr<std::thread> m_syncThread;

	std::shared_ptr<SceneDetourMgr> m_detour;

	CallSceneScripFunc m_callScriptFunc;

	void onPlayerEnter(GamePlayer* gamePlayer, std::set<int>& neighbours);
	void onNpcEnter(GameNpc* gameNpc, std::set<int>& neighbours);

	void onEnterSight(GameActor* actor, std::set<int>& enterIds);
	void onLeaveSight(GameActor* actor, std::set<int>& leaveIds);

	void _syncThreadFunc();
public:
	AOIMgr m_AOIMgr;
	int m_scriptObjId;
	int m_luaRef;

	inline int getSceneUid() { return m_sceneUid; }
	inline int getSceneId() { return m_sceneId; }
	GameScene(int sceneId, int sceneUid, void* scriptObj);

	inline void setCallScriptFunc(CallSceneScripFunc func) { m_callScriptFunc = func; }
	inline void* getScriptObject() { return m_scriptObj; }

	void onDestory();
	GamePlayer* createPlayer(int connId, int roleId, const char* name, int x, int y);
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
};


