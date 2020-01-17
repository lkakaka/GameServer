
import logger
from proto.message import message
from game.service.service_base import ServiceBase
import game.util.cmd_util
import game.scene.game_scene
import random
import game.scene.game_player


class SceneService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("s_scene")
    _c_cmd = game.util.cmd_util.CmdDispatch("c_scene")

    def __init__(self):
        ServiceBase.__init__(self)
        self._scenes = {}
        self._player_to_scene = {}

    def on_service_start(self):
        logger.logInfo("$Scene Service Start!!")
        self.add_scene(1)

    def add_scene(self, sceneId):
        scene = game.scene.game_scene.GameScene(self, sceneId)
        self._scenes[sceneId] = scene

    def get_player_scene(self, conn_id):
        scene_id = self._player_to_scene.get(conn_id)
        if scene_id is None:
            return None
        return self._scenes.get(scene_id)

    def on_recv_msg(self, sender, msg):
        logger.logInfo("$DBService on_recv_msg!!!")

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        logger.logInfo("$recv client msg, conn_id:{}, msg_id:{}", conn_id, msg_id)
        func = SceneService._c_cmd.get_cmd_func(msg_id)
        if func is not None:
            msg = message.create_msg_by_id(msg_id)
            msg.ParseFromString(msg_data)
            func(self, conn_id, msg_id, msg)
            return

        game_scene = self.get_player_scene(conn_id)
        if game_scene is None:
            logger.logInfo("$on_recv_client_msg error, player game_scene not found, conn_id:{}, msgId:{}", conn_id, msg_id)
            return
        game_scene.on_recv_client_msg(conn_id, msg_id, msg_data)

    def on_recv_service_msg(self, sender, msg_id, msg_data):
        logger.logInfo("$recv service msg, sender:{}, msg_id:{}", sender, msg_id)
        func = SceneService._s_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.logInfo("$on_recv_service_msg error, not found cmd func, msgId:{}", msg_id)
            return
        msg = message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func(self, sender, msg_id, msg)

    @_c_cmd.reg_cmd(message.MSG_ID_LOGIN_REQ)
    def _on_recv_login_req(self, conn_id, msg_id, msg):
        msg.conn_id = conn_id
        self.send_msg_to_service("db", msg_id, msg)

    @_c_cmd.reg_cmd(message.MSG_ID_TEST_REQ)
    def _on_recv_test_req(self, conn_id, msg_id, msg):
        rsp_msg = message.create_msg_by_id(message.MSG_ID_TEST_REQ)
        rsp_msg.id = 10
        rsp_msg.msg = "hello"
        self.send_msg_to_client(conn_id, message.MSG_ID_TEST_REQ, rsp_msg)

    @_s_cmd.reg_cmd(message.MSG_ID_LOGIN_RSP)
    def _on_recv_login_rsp(self, sender, msg_id, msg):
        if msg.err_code == 0:
            self.enter_scene(msg.conn_id, msg.user_id, "")
        conn_id = msg.conn_id
        msg.conn_id = 0
        self.send_msg_to_client(conn_id, message.MSG_ID_LOGIN_RSP, msg)

    def enter_scene(self, conn_id, role_id, name):
        scene = random.choice(list(self._scenes.values()))
        scene.on_player_enter(conn_id, role_id, name)
        self._player_to_scene[conn_id] = scene.scene_id

    def on_remove_player(self, conn_id):
        self._player_to_scene.pop(conn_id, None)


