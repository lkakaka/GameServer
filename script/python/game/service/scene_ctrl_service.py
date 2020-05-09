from game.service.service_base import ServiceBase
import game.util.cmd_util
import logger


class SceneCtrlService(ServiceBase):
    _s_cmd = game.util.cmd_util.CmdDispatch("scene_ctrl_service")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_scene_ctrl_service")

    def __init__(self):
        ServiceBase.__init__(self, SceneCtrlService._s_cmd, None, SceneCtrlService._rpc_proc)

    def on_service_start(self):
        logger.logInfo("$SceneCtrlService start!!!")

    @_rpc_proc.reg_cmd("RegScene")
    def _on_recv_rpc_reg_scene(self, sender, scene_id):
        print("_on_recv_reg_scene-----", scene_id)
        return "OK"
