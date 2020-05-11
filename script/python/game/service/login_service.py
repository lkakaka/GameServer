
from game.service.service_base import ServiceBase
from proto.pb_message import Message
import util.cmd_util
import util.const
from util import logger


class LoginService(ServiceBase):

    _s_cmd = util.cmd_util.CmdDispatch("s_login_service")
    _c_cmd = util.cmd_util.CmdDispatch("c_login_service")

    def __init__(self):
        ServiceBase.__init__(self, LoginService._s_cmd, LoginService._c_cmd)

    def on_service_start(self):
        logger.logInfo("$Login Service Start!!")

    @_c_cmd.reg_cmd(Message.MSG_ID_LOGIN_REQ)
    def _on_recv_login_req(self, conn_id, msg_id, msg):
        msg.conn_id = conn_id
        self.send_msg_to_service("db", msg_id, msg)

    @_s_cmd.reg_cmd(Message.MSG_ID_LOGIN_RSP)
    def _on_recv_login_rsp(self, sender, msg_id, msg):
        if msg.err_code == util.const.ErrorCode.OK:
            future = self.rpc_call("scene_ctrl", "QueryLoginScene", timeout=30, role_id=msg.user_id)
            future.finish_cb += self._on_query_login_scene
        # conn_id = msg.conn_id
        # msg.conn_id = 0
        # self.send_msg_to_client(conn_id, msg_id, msg)

    def _on_query_login_scene(self, err_code, scene_id, scene_uid):
        if err_code != util.const.ErrorCode.OK:
            pass
