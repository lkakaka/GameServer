
class GameActor(object):
    def __init__(self, actor_id):
        self.actor_id = actor_id

    def is_player(self):
        return False

    def is_npc(self):
        return False

    def check_can_see(self, actor):
        return True

    def on_actor_enter_sight(self, actor):
        print("on_actor_enter_sight")

    def on_actor_leave_sight(self, actor):
        print("on_actor_leave_sight")
