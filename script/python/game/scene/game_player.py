
import Scene
from proto.message import message
import game.util.cmd_util
import logger


class GamePlayer:

    _c_cmd = game.util.cmd_util.CmdDispatch("c_player")

    def __init__(self, e_player, game_scene, conn_id, role_id, name):
        self.native_obj = Scene.Player(e_player, self)
        self.actor_id = e_player[1]
        self.game_scene = game_scene
        self.conn_id = conn_id
        self.role_id = role_id
        self.name = name

    def on_recv_client_msg(self, msg_id, msg_data):
        msg = message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func = GamePlayer._c_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.logError("$player on_recv_client_msg error, not found cmd func, msgId:{}", msg_id)
            return
        func(self, msg_id, msg)

    def send_msg_to_client(self, msg_id, msg):
        self.game_scene.service.send_msg_to_client(self.conn_id, msg_id, msg)

    @_c_cmd.reg_cmd(message.MSG_ID_DISCONNECT)
    def _on_recv_disconnect(self, msg_id, msg):
        logger.logError("$player disconnect, role_id:{},reason:{}", self.role_id, msg.reason)
        self.game_scene.remove_player(self.role_id, msg.reason)
