#include "GameNpc.h"

GameNpc::GameNpc(int actorId, int npcId, int x, int y, int moveSpeed, void* gameScene, GridChgFunc gridChgFunc) :
	GameActor(ActorType::NPC, actorId, x, y, moveSpeed, gameScene, gridChgFunc), m_npcId(npcId)
{

}
