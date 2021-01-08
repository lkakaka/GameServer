#include "GameNpc.h"

GameNpc::GameNpc(int actorId, int npcId, void* gameScene, GridChgFunc gridChgFunc) :
	GameActor(ActorType::NPC, actorId, gameScene, gridChgFunc), m_npcId(npcId)
{

}
