#include "NpcEntity.h"
#include "Logger.h"

NpcEntity::NpcEntity(int eid, int npcId, int x, int y, int moveSpeed, void* gameScene, GridChgFunc gridChgFunc) :
	SceneEntity(SceneEntityType::NPC, eid, x, y, moveSpeed, gameScene, gridChgFunc), m_npcId(npcId)
{

}

void NpcEntity::moveTo(int x, int y) {
	std::vector<Position> tgtPosList;
	tgtPosList.emplace_back(x, y);
	setTgtPosList(tgtPosList);
	LOG_DEBUG("npc move to (%d, %d)", x, y);
}
