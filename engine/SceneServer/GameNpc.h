#pragma once
#include "GameActor.h"

class GameNpc : public GameActor
{
private:
	int m_npcId;
public:
	GameNpc(int actorId, int npcId, int x, int y, int moveSpeed, void* gameScene, GridChgFunc posChgFunc);

	void moveTo(int x, int y);
};

