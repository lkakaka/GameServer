
from game.service.service_base import ServiceBase
from proto.pb_message import Message
import util.cmd_util
from util.const import ErrorCode
from util import logger


class LoginService(ServiceBase):

    _s_cmd = util.cmd_util.CmdDispatch("s_login_service")
    _c_cmd = util.cmd_util.CmdDispatch("c_login_service")

    def __init__(self):
        ServiceBase.__init__(self, LoginService._s_cmd, LoginService._c_cmd)
        self._account_dict = {}
        self._conn_dict = {}    # 验证成功的连接

    def on_service_start(self):
        logger.logInfo("$Login Service Start!!")

    @_c_cmd.reg_cmd(Message.MSG_ID_LOGIN_REQ)
    def _on_recv_login_req(self, conn_id, msg_id, msg):
        # msg.conn_id = conn_id
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_LOGIN_RSP)
        # if msg.account in self._account_dict:
        #     rsp_msg.err_code = util.const.ErrorCode.ACCOUNT_IS_LOGINING
        #     self.send_msg_to_client(conn_id, MessageObj.MSG_ID_LOGIN_RSP, rsp_msg)
        #     return
        # todo:验证账号
        self._conn_dict[conn_id] = msg.account
        rsp_msg.err_code = util.const.ErrorCode.OK
        self.send_msg_to_client(conn_id, rsp_msg)

        self._account_dict[msg.account] = conn_id
        self._load_role_list(conn_id, msg.account)

    def _load_role_list(self, conn_id, account):
        msg = Message.create_msg_by_id(Message.MSG_ID_LOAD_ROLE_LIST_REQ)
        msg.account = account
        self.send_msg_to_service("db", msg)

        # rsp_msg = MessageObj.create_msg_by_id(MessageObj.MSG_ID_LOAD_ROLE_LIST_RSP)
        # rsp_msg.account = account
        # # db_res = self._db_handler.execute_sql("select * from player where account='{}'".format(msg.account))
        # # print("----", db_res)
        # # if db_res is None:
        # #     rsp_msg.err_code = util.const.ErrorCode.CREATE_PLAYER_ERROR
        # #     logger.logError("$create player error!!!")
        # # else:
        # # db_res = (())
        # rsp_msg.err_code = util.const.ErrorCode.OK
        # # for res in db_res:
        # role_info = rsp_msg.role_list.add()
        # role_info.role_id = 1#res.role_id
        # role_info.role_name = "test"#res.name
        # print("----role info", role_info)
        #     # rsp_msg.role_list.append(role_info)
        # print("send------", rsp_msg)
        # self.send_msg_to_service("db", MessageObj.MSG_ID_LOAD_ROLE_LIST_RSP, rsp_msg)
        # print("send------end")

    @_s_cmd.reg_cmd(Message.MSG_ID_LOAD_ROLE_LIST_RSP)
    def _on_recv_load_role_rsp(self, sender, msg_id, msg):
        conn_id = self._account_dict.pop(msg.account, None)
        if conn_id is None:
            logger.logError("$not found account's conn id, account:{}", msg.account)
            return
        self.send_msg_to_client(conn_id, msg)
        # if msg.err_code != util.const.ErrorCode.OK:
        #     self.send_msg_to_client(conn_id, msg_id, msg)
        #     return
        #
        # def _on_query_login_scene(err_code, scene_id=None, scene_uid=None):
        #     if err_code != util.const.ErrorCode.OK:
        #         msg.err_code = err_code
        #         self.send_msg_to_client(conn_id, msg_id, msg)
        #         return
        #
        # future = self.rpc_call("scene_ctrl", "QueryLoginScene", timeout=30, role_id=msg.user_id)
        # future.finish_cb += _on_query_login_scene
        # future.timeout_cb += _on_query_login_scene
        # conn_id = msg.conn_id
        # msg.conn_id = 0
        # self.send_msg_to_client(conn_id, msg_id, msg)

    @_c_cmd.reg_cmd(Message.MSG_ID_CREATE_ROLE_REQ)
    def _on_recv_create_role_req(self, conn_id, msg_id, msg):
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_CREATE_ROLE_RSP)
        account = self._conn_dict.get(conn_id)
        if account is None or account != msg.account:
            rsp_msg.err_code = ErrorCode.CONN_INVALID
            self.send_msg_to_client(conn_id, rsp_msg)
            logger.logError("create role error, account:{}, msg.account:{}", account, msg.account)
            return

        self.rpc_call("db", "CreateRole", conn_id=conn_id, account=msg.account, role_name=msg.role_name)

    @_c_cmd.reg_cmd(Message.MSG_ID_ENTER_GAME)
    def _on_recv_enter_game(self, conn_id, msg_id, msg):
        account = self._conn_dict.get(conn_id)
        if account is None:
            logger.logError("enter game error, conn_id({}) invalid", conn_id)
            self._send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
            return

        db_msg = Message.create_msg_by_id(Message.MSG_ID_LOAD_ROLE_REQ)
        db_msg.role_id = msg.role_id
        db_msg.conn_id = conn_id
        self.send_msg_to_service("db", db_msg)
        print("send enter game to db----")

    @_s_cmd.reg_cmd(Message.MSG_ID_LOAD_ROLE_RSP)
    def _on_recv_load_role_rsp(self, sender, msg_id, msg):
        print("_on_recv_load_role_rsp---")
        conn_id = msg.conn_id
        account = self._conn_dict.get(conn_id)
        if account is None:
            logger.logError("$load role rsp, conn_id({}) invalid", conn_id)
            self._send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
            return

        self._send_enter_game_rsp(conn_id, ErrorCode.OK, msg.role_info)

        def _on_query_login_scene(err_code, scene_id=None, scene_uid=None):
            if err_code != util.const.ErrorCode.OK:
                return

        future = self.rpc_call("scene_ctrl", "EnterScene", timeout=30, conn_id=conn_id, role_id=msg.role_info.role_id)
        future.finish_cb += _on_query_login_scene
        future.timeout_cb += _on_query_login_scene

    def _send_enter_game_rsp(self, conn_id, err_code, role_info):
        msg = Message.create_msg_by_id(Message.MSG_ID_ENTER_GAME_RSP)
        msg.err_code = err_code
        msg.role_info.CopyFrom(role_info)
        self.send_msg_to_client(conn_id, msg)
