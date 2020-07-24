#pragma once
#include <functional>
#include <vector>

enum ActorType
{
	UNKNOW = 0,
	PLYAER = 1,
	NPC = 2,
};

typedef struct Position {
	int x;
	int y;

	Position() : x(0), y(0) {}

	Position(int x, int y) : x(x), y(y) {}
}Position;

typedef std::function<void(int, Position*)> PosChgFunc;

class GameActor
{
protected:
	int m_actorId;
	int m_moveSpeed; 
	ActorType m_actorType;
	PosChgFunc m_posChgFunc;

	Position m_pos; // 当前位置
	std::vector<Position> m_tgtPosList;  // 移动的目标位置
public:

	GameActor(ActorType actorType, int actorId, PosChgFunc posChgFunc);
	GameActor(ActorType actorType, int actorId, int x, int y, PosChgFunc posChgFunc);

	inline int getActorId() { return m_actorId; }

	inline void setMoveSpeed(int speed) { m_moveSpeed = speed; }
	inline int getMoveSpeed() { return m_moveSpeed; }
	inline int getX() { return m_pos.x; }
	inline int getY() { return m_pos.y; }
	inline ActorType getActorType() { return m_actorType; }

	void setTgtPosList(std::vector<Position> tgtPosList);

	inline bool isMoving() {
		return !m_tgtPosList.empty();
	}

	void setPos(int x, int y);
	void updatePos();

	virtual void test() {};
};

