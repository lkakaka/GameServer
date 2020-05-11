from game.service.service_base import ServiceBase
import util.cmd_util
from util import logger
from util import const
import game.scene_ctrl.scene_mgr


class SceneCtrlService(ServiceBase):
    _s_cmd = util.cmd_util.CmdDispatch("scene_ctrl_service")
    _rpc_proc = util.cmd_util.CmdDispatch("rpc_scene_ctrl_service")

    def __init__(self):
        ServiceBase.__init__(self, SceneCtrlService._s_cmd, None, SceneCtrlService._rpc_proc)
        self._scene_mgr = game.scene_ctrl.scene_mgr.SceneMgr(self)

    def on_service_start(self):
        logger.logInfo("$SceneCtrlService start!!!")

    @_rpc_proc.reg_cmd("RegScene")
    def _on_recv_rpc_reg_scene(self, sender, scene_id=-1, scene_uid=-1):
        self._scene_mgr.reg_scene(scene_id, scene_uid)
        return const.ErrorCode.OK

    @_rpc_proc.reg_cmd("UnRegScene")
    def _on_recv_rpc_reg_scene(self, sender, scene_id=-1, scene_uid=-1):
        self._scene_mgr.unreg(scene_id, scene_uid)
        return const.ErrorCode.OK
