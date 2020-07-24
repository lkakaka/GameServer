#include "GameNpc.h"

GameNpc::GameNpc(int actorId, int npcId, PosChgFunc posChgFunc) :
	GameActor(ActorType::NPC, actorId, posChgFunc), m_npcId(npcId)
{

}
