# -*- encoding:utf-8 -*-
import Config
from game.util import logger
from proto.pb_message import Message
from game.service.service_base import ServiceBase
from game.service.service_addr import LOCAL_SCENE_CTRL_SERVICE_ADDR
import game.scene.game_scene
import game.scene.game_player
import game.util.timer
import game.util.const
import game.util.str_util
from game.gm.gm_handler import GMHandler


class SceneService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("s_scene")
    _c_cmd = game.util.cmd_util.CmdDispatch("c_scene")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_scene")

    def __init__(self):
        ServiceBase.__init__(self, SceneService._s_cmd, SceneService._c_cmd, SceneService._rpc_proc)
        self._scenes = {}
        self._player_to_scene = {}
        self.gm_handler = GMHandler(self)

    def on_service_start(self):
        ServiceBase.on_service_start(self)
        logger.log_info("Scene Service Start!!")
        scene_ids = Config.getConfigStr("scene_ids")
        # game.util.timer.add_timer(3, lambda _scene_id=1: self.create_scene(_scene_id))
        # game.util.timer.add_timer(3, lambda _scene_id=101: self.create_scene(_scene_id))
        for scene_id in game.util.str_util.parse_to_int_list(scene_ids):
            game.util.timer.add_timer(3, lambda _scene_id=scene_id: self.create_scene(_scene_id))

    def create_scene(self, scene_id):
        scene = game.scene.game_scene.GameScene(self, scene_id)
        self._scenes[scene.scene_uid] = scene

        def _reg_callback(err_code):
            if err_code == game.util.const.ErrorCode.OK:
                logger.log_info("reg scene success, result:{0}", err_code)
            else:
                logger.log_error("reg scene error, err_code:{0}, scene_id:{1}", err_code, scene_id)

        future = self.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "RegScene", timeout=60.0, scene_id=scene_id, scene_uid=scene.scene_uid)
        future.on_fin += _reg_callback
        future.on_timeout += _reg_callback
        logger.log_info("create scene, scene_id:{0}, scene_uid:{1}".format(scene_id, scene.scene_uid))

    def remove_scene(self, scene_uid):
        scene = self._scenes.pop(scene_uid, None)
        scene.on_destroy()

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
        logger.log_info("recv rpc enter scnene, conn_id:{0}, role_id{1}, scene_uid:{2}", conn_id, role_id, scene_uid)
        scene = self._scenes.get(scene_uid)
        self._player_to_scene[conn_id] = scene_uid
        scene.prepare_enter_scene(conn_id, role_id)

    @_s_cmd.reg_cmd(Message.MSG_ID_CLIENT_DISCONNECT)
    def _on_recv_disconnect(self, sender, msg_id, msg):
        game_scene = self.get_player_scene(msg.conn_id)
        if game_scene is None:
            logger.log_info("_on_recv_disconnect error, not found player's scene, conn_id:{}", msg.conn_id)
            return
        player = game_scene.get_player_by_conn_id(msg.conn_id)
        if player is None:
            logger.log_info("_on_recv_disconnect error, not found player in scene, conn_id:{}", msg.conn_id)
            return
        game_scene.remove_player(player.role_id, msg.reason)
        player.on_leave_game()

