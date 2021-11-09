#pragma once
#include <map>
#include "PlayerEntity.h"
#include "NpcEntity.h"
#include "AOIMgr.h"
#include <thread>
#include <mutex>
#include "DetourMgr.h"
#include "Vector.h"
#include "IScriptRegister.h"
#include "TaskMgr.h"

enum SceneScriptEvent {
	AFTER_ENTITY_ENTER = 1,
	AFTER_ENTITY_LEAVE = 2,
	AFTER_ENTITY_MOVE = 3,
};

class GameScene : public IScriptRegister
{
private:
	int m_sceneId;
	int m_sceneUid;
	std::map<int, SceneEntity*> m_entities;
	std::map<int, PlayerEntity*> m_players;
	int m_maxEntityId;
	std::shared_ptr<std::thread> m_syncThread;

	std::shared_ptr<SceneDetourMgr> m_detour;

	//std::shared_ptr<TaskMgr> m_syncTaskMgr;

	void onPlayerEnter(PlayerEntity* player, std::set<int>& neighbours);
	void onNpcEnter(NpcEntity* npc, std::set<int>& neighbours);

	void onEnterSight(SceneEntity* entity, std::set<int>& enterIds);
	void onLeaveSight(SceneEntity* entity, std::set<int>& leaveIds);

	void _syncThreadFunc();

public:
	AOIMgr m_AOIMgr;

	inline int getSceneUid() { return m_sceneUid; }
	inline int getSceneId() { return m_sceneId; }
	GameScene(int sceneId, int sceneUid);

	void onDestory();
	PlayerEntity* createPlayer(int connId, int roleId, const char* name, int x, int y, int moveSpeed);
	NpcEntity* createNpc(int npcId, int x, int y, int moveSpeed);
	void onEntityEnter(int eid);
	void onEntityLeave(SceneEntity* entity);
	void onEntityMove(SceneEntity* entity);
	SceneEntity* getEntity(int eid);
	PlayerEntity* getPlayer(int connId);
	void changePlayerConnId(int oldConnId, int connId);
	void removeEntity(int eid);
	void onCreate();

	void onEntityGridChg(int eid, Grid* pos);
	void onEntityPosChg(int eid, Position& pos);

	bool onRecvClientMsg(int connId, int msgId, char* data, int dataLen);

	bool loadNavMesh(const char* meshFileName);
	void findPath(float *sPos, float* ePos, std::vector<float>* path);

	void runLogicTask(std::function<void()> func);
	//void addSyncTask(std::function<void()> func);
};


