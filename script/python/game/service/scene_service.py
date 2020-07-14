from game.util import logger
from proto.pb_message import Message
from game.service.service_base import ServiceBase
import game.scene.game_scene
import game.scene.game_player
import game.util.timer
import game.util.const


class SceneService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("s_scene")
    _c_cmd = game.util.cmd_util.CmdDispatch("c_scene")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_scene")

    def __init__(self):
        ServiceBase.__init__(self, SceneService._s_cmd, SceneService._c_cmd, SceneService._rpc_proc)
        self._scenes = {}
        self._player_to_scene = {}

    def on_service_start(self):
        ServiceBase.on_service_start(self)
        logger.log_info("Scene Service Start!!")
        game.util.timer.add_timer(3, lambda: self.create_scene(1))

    def create_scene(self, scene_id):
        scene = game.scene.game_scene.GameScene(self, scene_id)
        self._scenes[scene.scene_uid] = scene

        def _reg_callback(err_code):
            if err_code == game.util.const.ErrorCode.OK:
                logger.log_info("reg scene success, result:{}", err_code)
            else:
                logger.log_error("reg scene error, err_code:{}, scene_id:{}", err_code, scene_id)

        future = self.rpc_call("scene_ctrl", "RegScene", timeout=10.0, scene_id=scene_id, scene_uid=scene.scene_uid)
        future.on_fin += _reg_callback
        future.on_timeout += _reg_callback

    def get_player_scene(self, conn_id):
        scene_id = self._player_to_scene.get(conn_id)
        if scene_id is None:
            return None
        return self._scenes.get(scene_id)

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        logger.log_info("recv client msg, conn_id:{}, msg_id:{}", conn_id, msg_id)
        func = SceneService._c_cmd.get_cmd_func(msg_id)
        if func is not None:
            msg = Message.create_msg_by_id(msg_id)
            msg.ParseFromString(msg_data)
            func(self, conn_id, msg_id, msg)
            return

        game_scene = self.get_player_scene(conn_id)
        if game_scene is None:
            logger.log_info("on_recv_client_msg error, player game_scene not found, conn_id:{}, msgId:{}", conn_id, msg_id)
            return
        game_scene.on_recv_client_msg(conn_id, msg_id, msg_data)

    # def on_recv_service_msg(self, sender, msg_id, msg_data):
    #     logger.logInfo("$recv service msg, sender:{}, msg_id:{}", sender, msg_id)
    #     func = SceneService._s_cmd.get_cmd_func(msg_id)
    #     if func is None:
    #         logger.logInfo("$on_recv_service_msg error, not found cmd func, msgId:{}", msg_id)
    #         return
    #     msg = Message.create_msg_by_id(msg_id)
    #     msg.ParseFromString(msg_data)
    #     func(self, sender, msg_id, msg)

    def on_remove_player(self, conn_id):
        self._player_to_scene.pop(conn_id, None)

    @_rpc_proc.reg_cmd("Scene_EnterScene")
    def _on_recv_rpc_enter_scene(self, sender, conn_id, role_id, scene_uid):
        scene = self._scenes.get(scene_uid)
        self._player_to_scene[conn_id] = scene_uid
        scene.prepare_enter_scene(conn_id, role_id)

    @_s_cmd.reg_cmd(Message.MSG_ID_CLIENT_DISCONNECT)
    def _on_recv_disconnect(self, sender, msg_id, msg):
        game_scene = self.get_player_scene(msg.conn_id)
        player = game_scene.get_player_by_conn_id(msg.conn_id)
        game_scene.remove_player(player.role_id, msg.reason)

