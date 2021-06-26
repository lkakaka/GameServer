# -*- encoding:utf-8 -*-
from game.service.service_base import ServiceBase
from game.util import logger
from game.util.const import ErrorCode
import game.util.cmd_util
import game.scene_ctrl.scene_mgr
from game.scene_ctrl.player_mgr import PlayerState


class SceneCtrlService(ServiceBase):
    _s_cmd = game.util.cmd_util.CmdDispatch("scene_ctrl_service")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_scene_ctrl_service")

    def __init__(self):
        ServiceBase.on_service_start(self)
        ServiceBase.__init__(self, SceneCtrlService._s_cmd, None, SceneCtrlService._rpc_proc)
        self._scene_mgr = game.scene_ctrl.scene_mgr.SceneMgr(self)
        self._player_mgr = game.scene_ctrl.player_mgr.PlayerMgr()

    def on_service_start(self):
        logger.log_info("SceneCtrlService start!!!")

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
        player_info = self._player_mgr.get_player_info_by_role_id(role_id)
        player_info.state = PlayerState.SWITCHING
        player_info.scene_uid = 0
        player_info.scene_id = 0
        logger.log_info("player switch scene, role_id:{0}, scene_uid:{1}", role_id, scene_uid)
        self.rpc_call(scene.service_addr, "Scene_EnterScene", conn_id=conn_id, role_id=role_id, scene_uid=scene_uid)
        return ErrorCode.OK

    @_rpc_proc.reg_cmd("Player_EnterScene")
    def _on_player_enter_scene(self, sender, role_id, scene_uid, scene_id):
        player_info = self._player_mgr.get_player_info_by_role_id(role_id)
        player_info.state = PlayerState.IN_SCENE
        player_info.scene_uid = scene_uid
        player_info.scene_id = scene_id
