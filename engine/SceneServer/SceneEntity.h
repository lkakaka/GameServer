#pragma once
#include <functional>
#include <vector>
#include <set>
#include "Vector.h"
#include <mutex>
#include "proto.h"
//#include "GameScene.h"

enum SceneEntityType
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

class SceneEntity
{
protected:
	int m_entityId;
	SceneEntityType m_entityType;
	int m_moveSpeed;
	void* m_gameScene;
	std::set<int> m_sightEntityIds; // 视野中的角色
	Position m_pos; // 当前位置
	Grid m_grid; // 所处格子
	std::vector<Position> m_tgtPosList;  // 移动的目标位置
	std::vector<Position> m_srcTgtPosList;  // 移动的目标位置
	std::mutex m_tgtPosLock;
	int64_t m_lastMoveTime;	// 上次计算移动的时间戳(更新位置是使用)

	int m_viewRange;

	GridChgFunc m_gridChgFunc;

public:

	SceneEntity(SceneEntityType eType, int eid, void* gameScene, GridChgFunc posChgFunc);
	SceneEntity(SceneEntityType eType, int eid, int x, int y, int moveSpeed, void* gameScene, GridChgFunc posChgFunc);

	virtual ~SceneEntity() {}

	inline int getEntityId() { return m_entityId; }

	inline void setMoveSpeed(int speed) { m_moveSpeed = speed; }
	inline int getMoveSpeed() { return m_moveSpeed; }
	inline int getX() { return m_pos.x; }
	inline int getY() { return m_pos.y; }
	inline int getGridX() { return m_grid.x; }
	inline int getGridY() { return m_grid.y; }
	inline int getViewRange() { return m_viewRange; }
	inline SceneEntityType getEntityType() { return m_entityType; }
	inline bool isPlayer() { return m_entityType == PLYAER; }
	inline bool isNpc() { return m_entityType == NPC; }

	void addSightEntities(std::set<int>& entities);
	void addSightEntity(int eid);
	void removeSightEntities(std::set<int>& entities);
	void removeSightEntity(int eid);
	std::set<int> getSightEntityConndIds();

	void setTgtPosList(std::vector<Position>& tgtPosList);
	void updatePos(int64_t ts);

	void setPos(float x, float y, bool isTemp=false);

	virtual void test() {};

	void broadcastMsgToClient(std::set<int>& connIds, int msgId, google::protobuf::Message* msg);
	void broadcastMsgToClient(std::set<int>& connIds, int msgId, const char* msg, int msgLen);
	void broadcastMsgToSight(int msgId, google::protobuf::Message* msg);
	void broadcastMsgToSight(int msgId, const char* msg, int msgLen);
};

