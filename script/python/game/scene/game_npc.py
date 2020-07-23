
import game.scene.game_actor


class GameNpc(game.scene.game_actor.GameActor):
    def __init__(self, actor_id, npc_id):
        game.scene.game_actor.GameActor.__init__(self, actor_id)
        self.npc_id = npc_id

    def pack_born_info(self, msg):
        npc_info = msg.npc_list.add()
        npc_info.actor_id = self.actor_id
        npc_info.npc_id = self.npc_id
