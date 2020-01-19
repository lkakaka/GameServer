#pragma once
class GameActor
{
private:
	int m_actorId;
public:

	GameActor(int actorId);

	inline int getActorId() { return m_actorId; }

	virtual void test() {};
};

