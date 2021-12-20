# -*- encoding:utf-8 -*-
from game.service.service_base import ServiceBase
from game.util import logger
from game.util.const import ErrorCode
from game.util.const import TokenPrefix
import game.util.cmd_util
import game.scene_ctrl.scene_mgr
from game.scene_ctrl.player_mgr import PlayerState
import random
import time
from game.service.service_addr import LOCAL_LOGIN_SERVICE_ADDR
from game.service.service_addr import LOCAL_SERVICE_GROUP
import Crypt


class SceneCtrlService(ServiceBase):
    _s_cmd = game.util.cmd_util.CmdDispatch("scene_ctrl_service")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_scene_ctrl_service")

    def __init__(self):
        ServiceBase.on_service_start(self)
        ServiceBase.__init__(self, SceneCtrlService._s_cmd, None, SceneCtrlService._rpc_proc)
        self._scene_mgr = game.scene_ctrl.scene_mgr.SceneMgr(self)
        self._player_mgr = game.scene_ctrl.player_mgr.PlayerMgr()
        self._gateway_addresses = {}
        self._cross_server_roles = {}
        self._local_cross_player_info = {}

    def on_service_start(self):
        logger.log_info("SceneCtrlService start!!!")

    @_rpc_proc.reg_cmd("RegGatewayAddress")
    def _on_reg_gateway_addr(self, sender, ip, port, udp_port):
        self._gateway_addresses[sender.service_id] = {'ip': ip, 'port': port, 'udp_port': udp_port}
        logger.log_info("rpc_RegGatewayAddress, ip:{},port:{},udp_port:{}, {}", ip, port, udp_port, self._gateway_addresses)
        return ErrorCode.OK

    @_rpc_proc.reg_cmd("RegScene")
    def _on_recv_rpc_reg_scene(self, sender, scene_id, scene_uid):
        self._scene_mgr.reg_scene(scene_id, scene_uid, sender)
        return ErrorCode.OK

    @_rpc_proc.reg_cmd("UnRegScene")
    def _on_recv_rpc_unreg_scene(self, sender, scene_uid):
        self._scene_mgr.unreg_scene(scene_uid)
        return ErrorCode.OK

    @_rpc_proc.reg_cmd("Player_EnterGame")
    def _on_player_enter_game(self, sender, conn_id, role_id, scene_id=1):
        scene = self._scene_mgr.get_min_player_scene(scene_id)
        if scene is None:
            logger.log_info("player enter game failed, not found scene, scene_id:{0}, role_id:{1}", scene_id, role_id)
            return ErrorCode.NOT_FOUND_SCENE
        player_info = self._player_mgr.add_player(role_id, conn_id)
        player_info.state = PlayerState.LOGINING
        logger.log_info("player enter game, role_id:{0}, scene_id:{1}, scene_uid:{2}", role_id, scene_id, scene.scene_uid)
        self.rpc_call(scene.service_addr, "Scene_EnterScene", conn_id=conn_id, role_id=role_id, scene_uid=scene.scene_uid)
        return ErrorCode.OK

    @_rpc_proc.reg_cmd("Player_LeaveGame")
    def _on_player_leave_game(self, sender, role_id):
        self._player_mgr.remove_player(role_id)
        logger.log_info("player leave game, role_id:{0}", role_id)

    @_rpc_proc.reg_cmd("Player_SwitchSceneReq")
    def _on_player_switch_scene_req(self, sender, role_id, scene_id):
        scene = self._scene_mgr.get_min_player_scene(scene_id)
        if scene is None:
            logger.log_info("player switch scene req failed, not found scene, scene_id:{0}, role_id:{1}", scene_id, role_id)
            return ErrorCode.NOT_FOUND_SCENE, -1
        logger.log_info("player switch scene req, role_id:{0}, scene_id:{1}, scene_uid:{2}", role_id, scene_id, scene.scene_uid)
        return ErrorCode.OK, scene.scene_uid

    @_rpc_proc.reg_cmd("Player_SwitchScene")
    def _on_player_switch_scene(self, sender, conn_id, role_id, scene_uid):
        scene = self._scene_mgr.get_scene_by_uid(scene_uid)
        if scene is None:
            logger.log_info("player switch scene failed, not found scene, scene_uid:{0}, role_id:{1}", scene_uid,
                            role_id)
            return ErrorCode.NOT_FOUND_SCENE
        self._player_mgr.switch_state(role_id, PlayerState.SWITCHING, scene_uid=0, scene_id=0)
        logger.log_info("player switch scene, role_id:{0}, scene_uid:{1}", role_id, scene_uid)
        self.rpc_call(scene.service_addr, "Scene_EnterScene", conn_id=conn_id, role_id=role_id, scene_uid=scene_uid)
        return ErrorCode.OK

    @_rpc_proc.reg_cmd("Player_EnterScene")
    def _on_player_enter_scene(self, sender, role_id, scene_uid, scene_id):
        self._player_mgr.switch_state(role_id, PlayerState.IN_SCENE, scene_uid=scene_uid, scene_id=scene_id)

    @_rpc_proc.reg_cmd("Player_RemoteSwitchSceneReq")
    def rpc_remote_switch_scene_req(self, sender, role_id, scene_id):
        scene = self._scene_mgr.get_min_player_scene(scene_id)
        if scene is None:
            logger.log_info("player switch scene req failed, not found scene, scene_id:{}, role_id:{}", scene_id, role_id)
            return ErrorCode.NOT_FOUND_SCENE, -1
        rand_num = random.randint(1, 10000)
        str_token = "{}_{}_{}_{}".format(TokenPrefix.CROSS_SERVER, role_id, time.time(), rand_num)
        str_token = Crypt.md5(str_token)
        token_valid_ts = time.time() + 30
        self._cross_server_roles[role_id] = {'token': str_token, 'token_ts': token_valid_ts, 'scene_uid': scene.scene_uid}
        gateway_addr = random.choice(self._gateway_addresses)
        logger.log_info("player switch scene req, role_id:{}, scene_id:{}, scene_uid:%d, gateway_addr:{}",
                       role_id, scene_id, scene.scene_uid, gateway_addr)
        future = self.rpc_call(LOCAL_LOGIN_SERVICE_ADDR, "regRemoteRole", -1, role_id=role_id, token=str_token, token_ts=token_valid_ts)

        def _on_reg_remote_role_cb(err_code):
            if err_code == ErrorCode.OK:
                return
            logger.log_error("reg remote role to login failed, role_id:{}", role_id)

        future.on_fin += _on_reg_remote_role_cb
        future.on_timeout += _on_reg_remote_role_cb

        return ErrorCode.OK, LOCAL_SERVICE_GROUP, scene.scene_uid, gateway_addr, str_token

    @_rpc_proc.reg_cmd("Player_RemoteSwitchScene")
    def rpc_remote_switch_scene(self, sender, role_id, scene_uid):
        scene = self._scene_mgr.get_scene_by_uid(scene_uid)
        if scene is None:
            logger.log_info("player remote switch scene failed, not found scene, scene_uid:{}, role_id:{}", scene_uid, role_id)
            return ErrorCode.NOT_FOUND_SCENE

        self._player_mgr.switch_state(role_id, PlayerState.REMOTE_SWITCHING, scene_uid=0, scene_id=0)
        logger.log_info("player remote switch scene, role_id:{}, scene_uid:{}", role_id, scene_uid)
        return ErrorCode.OK

    @_rpc_proc.reg_cmd("Player_SwitchToRemoteScene")
    def rpc_switch_to_remote_scene(self, sender, role_id, server_id, scene_uid):
        self._local_cross_player_info[role_id] = {"remote_server_id": server_id, "scene_uid": scene_uid}
        logger.log_info("player switch to remote scene, role_id:{}, remote_server_id:{} scene_uid:{}", role_id, server_id, scene_uid)

    @_rpc_proc.reg_cmd("Player_RemoteEnterGame")
    def rpc_remote_enter_game(self, sender, conn_id, role_id):
        player_info = self._player_mgr.get_player_info_by_role_id(role_id)
        # 玩家在场景中
        if player_info is not None:
            if player_info.state == PlayerState.IN_SCENE:
                logger.log_info("remote player already entered game, role_id:{}, scene_uid:{}", role_id, player_info.scene_uid)
                # player_info.change_conn_id(conn_id)
                player_info.conn_id = conn_id
                scene = self._scene_mgr.get_scene_by_uid(player_info.scene_uid)
                self.rpc_call(scene.service_addr, "Scene_EnterScene", -1, conn_id=conn_id, role_id=role_id, scene_uid=player_info.scene_uid)
                return ErrorCode.OK
            logger.log_error("remote player status error, role_id:{}, state:{}", role_id, player_info.state)
            return ErrorCode.SYS_ERROR

        cross_info = self._cross_server_roles.pop(role_id, None)
        if cross_info is None:
            logger.log_info("player remote enter game failed, not found cross server token, role_id:{}", role_id)
            return ErrorCode.NOT_FOUND_SCENE

        scene = self._scene_mgr.get_scene_by_uid(cross_info["scene_uid"])
        player_info = self._player_mgr.add_player(role_id, conn_id)
        player_info.state = PlayerState.LOGINING
        logger.log_info("player remote enter game, role_id:{}, scene_id:{}, scene_uid:{}", role_id, scene.scene_id, scene.scene_uid)
        self.rpc_call(scene.service_addr, "Scene_EnterScene", -1, conn_id=conn_id, role_id=role_id, scene_uid=scene.scene_uid)
        return ErrorCode.OK

