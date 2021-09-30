#pragma once
#include <functional>
#include <vector>
#include <set>
#include "Vector.h"
#include <mutex>
#include "proto.h"
//#include "GameScene.h"

enum ActorType
{
	UNKNOW = 0,
	PLYAER = 1,
	NPC = 2,
};

#define GRID_X_SIZE 10
#define GRID_Y_SIZE 10

typedef Vector2<float> Position;
typedef Vector2<int> Grid;

typedef std::function<void(int, Grid*)> GridChgFunc;

class GameActor
{
protected:
	int m_actorId;
	ActorType m_actorType;
	int m_moveSpeed;
	void* m_gameScene;
	std::set<int> m_sightActors; // 视野中的角色
	Position m_pos; // 当前位置
	Grid m_grid; // 所处格子
	std::vector<Position> m_tgtPosList;  // 移动的目标位置
	std::vector<Position> m_srcTgtPosList;  // 移动的目标位置
	std::mutex m_tgtPosLock;
	int64_t m_lastMoveTime;	// 上次计算移动的时间戳(更新位置是使用)

	GridChgFunc m_gridChgFunc;

public:

	GameActor(ActorType actorType, int actorId, void* gameScene, GridChgFunc posChgFunc);
	GameActor(ActorType actorType, int actorId, int x, int y, int moveSpeed, void* gameScene, GridChgFunc posChgFunc);

	virtual ~GameActor() {}

	inline int getActorId() { return m_actorId; }

	inline void setMoveSpeed(int speed) { m_moveSpeed = speed; }
	inline int getMoveSpeed() { return m_moveSpeed; }
	inline int getX() { return m_pos.x; }
	inline int getY() { return m_pos.y; }
	inline int getGridX() { return m_grid.x; }
	inline int getGridY() { return m_grid.y; }
	inline ActorType getActorType() { return m_actorType; }
	inline bool isPlayer() { return m_actorType == PLYAER; }
	inline bool isNpc() { return m_actorType == NPC; }

	void addSightActors(std::set<int>& actors);
	void addSightActor(int actorId);
	void removeSightActors(std::set<int>& actors);
	void removeSightActor(int actorId);
	std::set<int> getSightActorConndIds();

	void setTgtPosList(std::vector<Position>& tgtPosList);
	void updatePos(int64_t ts);

	void setPos(float x, float y, bool isTemp=false);

	virtual void test() {};

	void broadcastMsgToClient(std::set<int>& connIds, int msgId, google::protobuf::Message* msg);
	void broadcastMsgToClient(std::set<int>& connIds, int msgId, const char* msg, int msgLen);
	void broadcastMsgToSight(int msgId, google::protobuf::Message* msg);
	void broadcastMsgToSight(int msgId, const char* msg, int msgLen);
};

