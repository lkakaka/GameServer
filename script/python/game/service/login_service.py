
from game.service.service_base import ServiceBase
import game.util.cmd_util
from proto.message import message


class LoginService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("s_login")
    _c_cmd = game.util.cmd_util.CmdDispatch("c_login")

    def __init__(self):
        ServiceBase.__init__(self)

    @_c_cmd.reg_cmd(message.MSG_ID_LOGIN_REQ)
    def _on_recv_login_req(self, conn_id, msg_id, msg):
        msg.conn_id = conn_id
        self.send_msg_to_service("db", msg_id, msg)
