
import Scene
from proto.pb_message import Message
import util.cmd_util
from util import logger


class GamePlayer:

    _c_cmd = util.cmd_util.CmdDispatch("c_player")

    def __init__(self, e_player, game_scene, conn_id, role_id, name):
        self.native_obj = Scene.Player(e_player, self)
        self.actor_id = e_player[1]
        self.game_scene = game_scene
        self.conn_id = conn_id
        self.role_id = role_id
        self.name = name

    def on_recv_client_msg(self, msg_id, msg_data):
        msg = Message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func = GamePlayer._c_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.logError("$player on_recv_client_msg error, not found cmd func, msgId:{}", msg_id)
            return
        func(self, msg_id, msg)

    def send_msg_to_client(self, msg):
        self.game_scene.service.send_msg_to_client(self.conn_id, msg)

    @_c_cmd.reg_cmd(Message.MSG_ID_DISCONNECT)
    def _on_recv_disconnect(self, msg_id, msg):
        logger.logError("$player disconnect, role_id:{},reason:{}", self.role_id, msg.reason)
        self.game_scene.remove_player(self.role_id, msg.reason)

    @_c_cmd.reg_cmd(Message.MSG_ID_TEST_REQ)
    def _on_recv_test_req(self, msg_id, msg):
        print("$player recv test req, role_id:{}, msg:{}", self.role_id, msg)
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_TEST_REQ)
        rsp_msg.id = 10
        rsp_msg.msg = "welcome to game world, " + self.name
        self.send_msg_to_client(rsp_msg)
