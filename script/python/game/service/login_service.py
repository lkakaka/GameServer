
from game.service.service_base import ServiceBase
from proto.message import Message
import game.util.cmd_util
import logger


class LoginService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("s_login_service")
    _c_cmd = game.util.cmd_util.CmdDispatch("c_login_service")

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
        # if msg.err_code == 0:
        #     self.on_player_load(msg.conn_id, msg.user_id, "")
        conn_id = msg.conn_id
        msg.conn_id = 0
        self.send_msg_to_client(conn_id, msg_id, msg)
