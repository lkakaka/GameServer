#pragma once
#include "GameActor.h"

class GameNpc : public GameActor
{
private:
	int m_npcId;
public:
	GameNpc(int actorId, int npcId, void* gameScene, GridChgFunc posChgFunc);
};

