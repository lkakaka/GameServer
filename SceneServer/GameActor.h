#pragma once
#include <functional>
#include <vector>
#include <set>
#include "Vector.h"

enum ActorType
{
	UNKNOW = 0,
	PLYAER = 1,
	NPC = 2,
};

typedef std::function<void(int, Vector<int>*)> PosChgFunc;

class GameActor
{
protected:
	int m_actorId;
	int m_moveSpeed; 
	ActorType m_actorType;
	PosChgFunc m_posChgFunc;

	std::set<int> m_sightActors; // ��Ұ�еĽ�ɫ

	Vector<int> m_pos; // ��ǰλ��
	std::vector<Vector<int>> m_tgtPosList;  // �ƶ���Ŀ��λ��
	int64_t m_lastMoveTime;	// �ϴμ����ƶ���ʱ���(����λ����ʹ��)
public:

	GameActor(ActorType actorType, int actorId, PosChgFunc posChgFunc);
	GameActor(ActorType actorType, int actorId, int x, int y, PosChgFunc posChgFunc);

	inline int getActorId() { return m_actorId; }

	inline void setMoveSpeed(int speed) { m_moveSpeed = speed; }
	inline int getMoveSpeed() { return m_moveSpeed; }
	inline int getX() { return m_pos.x; }
	inline int getY() { return m_pos.y; }
	inline ActorType getActorType() { return m_actorType; }

	void addSightActors(std::set<int>& actors);
	void removeSightActors(std::set<int>& actors);

	void setTgtPosList(std::vector<Vector<int>> tgtPosList);

	inline bool isMoving() {
		return !m_tgtPosList.empty();
	}

	void setPos(int x, int y);
	void updatePos(int64_t ts);

	virtual void test() {};
};

