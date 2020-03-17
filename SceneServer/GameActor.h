#pragma once

enum ActorType
{
	UNKNOW = 0,
	PLYAER = 1,
	NPC = 2,
};

class GameActor
{
private:
	int m_actorId;
	int x, y;
	int m_moveSpeed; 
	ActorType m_actorType;
public:

	GameActor(ActorType actorType, int actorId);
	GameActor(ActorType actorType, int actorId, int x, int y);

	inline int getActorId() { return m_actorId; }

	inline void setMoveSpeed(int speed) { m_moveSpeed = speed; }
	inline int getMoveSpeed() { return m_moveSpeed; }
	inline int getX() { return x; }
	inline int getY() { return y; }
	inline ActorType getActorType() { return m_actorType; }

	void setPos(int x, int y);

	virtual void test() {};
};

