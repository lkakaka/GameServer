#pragma once
#include "SceneEntity.h"

class NpcEntity : public SceneEntity
{
private:
	int m_npcId;
public:
	NpcEntity(int eid, int npcId, int x, int y, int moveSpeed, void* gameScene, GridChgFunc posChgFunc);

	void moveTo(int x, int y);
};

