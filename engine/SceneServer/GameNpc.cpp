#include "GameNpc.h"
#include "Logger.h"

GameNpc::GameNpc(int actorId, int npcId, int x, int y, int moveSpeed, void* gameScene, GridChgFunc gridChgFunc) :
	GameActor(ActorType::NPC, actorId, x, y, moveSpeed, gameScene, gridChgFunc), m_npcId(npcId)
{

}

void GameNpc::moveTo(int x, int y) {
	std::vector<Position> tgtPosList;
	tgtPosList.emplace_back(x, y);
	setTgtPosList(tgtPosList);
	LOG_DEBUG("npc move to (%d, %d)", x, y);
}
