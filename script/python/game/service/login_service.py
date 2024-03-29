﻿# -*- encoding:utf-8 -*-

from game.service.service_base import ServiceBase
from game.service.service_addr import ServiceAddr
from game.service.service_addr import LOCAL_DB_SERVICE_ADDR
from game.service.service_addr import LOCAL_SCENE_CTRL_SERVICE_ADDR
from proto.pb_message import Message
from game.util.const import ErrorCode
from game.util.const import TokenPrefix
from game.util import logger
import game.util.cmd_util
import Config
import Crypt
import time
import random


class LoginService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("s_login_service")
    _c_cmd = game.util.cmd_util.CmdDispatch("c_login_service")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_login_service")

    def __init__(self):
        ServiceBase.on_service_start(self)
        ServiceBase.__init__(self, LoginService._s_cmd, LoginService._c_cmd, LoginService._rpc_proc)
        self._account_dict = {}
        self._conn_dict = {}    # 验证成功的连接
        self._remote_login_roles = {}

        port = Config.getConfigInt("http_server_port")
        if port:
            import game.login.login_http_server
            self._http_server = game.login.login_http_server.LoginHttpServer(self, port)

    def on_service_start(self):
        logger.log_info("Login Service Start!!")

    @_c_cmd.reg_cmd(Message.MSG_ID_LOGIN_REQ)
    def _on_recv_login_req(self, conn_id, msg_id, msg):
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_LOGIN_RSP)
        # if msg.account in self._account_dict:
        #     rsp_msg.err_code = util.const.ErrorCode.ACCOUNT_IS_LOGINING
        #     self.send_msg_to_client(conn_id, MessageObj.MSG_ID_LOGIN_RSP, rsp_msg)
        #     return
        # todo:验证账号
        self._conn_dict[conn_id] = msg.account
        logger.log_info("recv login req conn_id={}", conn_id)
        rsp_msg.err_code = game.util.const.ErrorCode.OK
        self.send_msg_to_client(conn_id, rsp_msg)

        self._account_dict[msg.account] = conn_id
        self._load_role_list(conn_id, msg.account)

    def _load_role_list(self, conn_id, account):
        def on_load_role_list(err_code, tbls=[]):
            print("on_load_role_list---", tbls)
            self._on_load_role_list(account, err_code, tbls)

        future = self.db_proxy.load(-1, "player", account=account)
        future.on_fin += on_load_role_list
        future.on_timeout += on_load_role_list

    def _on_load_role_list(self, account, err_code, tbls):
        conn_id = self._account_dict.pop(account, None)
        if conn_id is None:
            logger.log_error("not found account's conn id, account:{}", account)
            return
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_LOAD_ROLE_LIST_RSP)
        rsp_msg.account = account
        rsp_msg.err_code = err_code
        for tbl in tbls:
            role_info = rsp_msg.role_list.add()
            role_info.role_id = tbl["role_id"]
            role_info.role_name = tbl["role_name"]
        self.send_msg_to_client(conn_id, rsp_msg)

    @_c_cmd.reg_cmd(Message.MSG_ID_CREATE_ROLE_REQ)
    def _on_recv_create_role_req(self, conn_id, msg_id, msg):
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_CREATE_ROLE_RSP)
        account = self._conn_dict.get(conn_id)
        if account is None or account != msg.account:
            rsp_msg.err_code = ErrorCode.CONN_INVALID
            self.send_msg_to_client(conn_id, rsp_msg)
            logger.log_error("create role error, account:{}, msg.account:{}", account, msg.account)
            return

        self.rpc_call(LOCAL_DB_SERVICE_ADDR, "CreateRole", conn_id=conn_id, account=msg.account, role_name=msg.role_name)

    @_c_cmd.reg_cmd(Message.MSG_ID_ENTER_GAME)
    def _on_recv_enter_game(self, conn_id, msg_id, msg):
        account = self._conn_dict.get(conn_id)
        if account is None:
            logger.log_error("enter game error, conn_id({}) invalid", conn_id)
            self._send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
            return

        def on_load_role(err_code, tbls):
            self._on_load_role(conn_id, err_code, tbls)

        future = self.db_proxy.load(-1, "player", role_id=msg.role_id)
        future.on_fin += on_load_role
        future.on_timeout += on_load_role
        logger.log_info("enter game req, conn_id:{}, account:{}", conn_id, account)

    def _on_load_role(self, conn_id, err_code, tbls):
        account = self._conn_dict.pop(conn_id, None)
        if account is None:
            logger.log_error("load role rsp, conn_id({}) invalid", conn_id)
            self._send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
            return

        if not tbls:
            logger.log_error("load role rsp, role data not exist, account:{}", account)
            self._send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
            return

        tbl = tbls[0]

        def _on_query_login_scene(error_code, scene_id=None, scene_uid=None):
            if error_code != game.util.const.ErrorCode.OK:
                self._send_enter_game_rsp(conn_id, error_code)
                return
            self._send_enter_game_rsp(conn_id, ErrorCode.OK, tbl)
            self._send_kcp_start_info(conn_id)

        future = self.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "Player_EnterGame", timeout=30,
                               conn_id=conn_id, role_id=tbl["role_id"])
        future.on_fin += _on_query_login_scene
        future.on_timeout += _on_query_login_scene
        logger.log_info("send enter scene req to scene ctrl, conn_id:{}, account:{}", conn_id, account)

    def _send_enter_game_rsp(self, conn_id, err_code, tbl_player=None):
        msg = Message.create_msg_by_id(Message.MSG_ID_ENTER_GAME_RSP)
        msg.err_code = err_code
        if tbl_player is not None:
            msg.role_info.role_id = tbl_player["role_id"]
            msg.role_info.role_name = tbl_player["role_name"]
        self.send_msg_to_client(conn_id, msg)

    def _send_kcp_start_info(self, conn_id):
        rand_num = random.randint(1, 10000)
        str_token = "{0}_{1}_{2}_{3}".format(TokenPrefix.KCP, conn_id, time.time(), rand_num)
        msg = Message.create_msg_by_id(Message.MSG_ID_START_KCP)
        msg.kcp_id = conn_id
        msg.token = Crypt.md5(str_token)
        self.send_msg_to_client(conn_id, msg)

    @_rpc_proc.reg_cmd("regRemoteRole")
    def rpc_reg_remote_role(self, sender, role_id, token, token_ts):
        self._remote_login_roles[role_id] = {"token": token, "token_ts": token_ts}
        logger.log_info("recv reg remote role, role_id:{}, ts:{}", role_id, token_ts)
        return ErrorCode.OK

    @_c_cmd.reg_cmd(Message.MSG_ID_REMOTE_ENTER_GAME)
    def remote_enter_game(self, conn_id, msg_id, msg):
        role_id = msg.role_id
        remote_login_info = self._remote_login_roles.pop(role_id, None)
        if remote_login_info is None:
            logger.log_error("remote enter game error, role_id:{}", role_id)
            self._send_enter_game_rsp(conn_id, ErrorCode.CROSS_ROLE_ERROR)
            return

        if remote_login_info["token"] != msg.token:
            logger.log_error("remote login token error, role_id:{}", role_id)
            self._send_enter_game_rsp(conn_id, ErrorCode.CROSS_TOKEN_ERROR)
            return

        if remote_login_info["token_ts"] < time.time():
            logger.log_error("remote login token expired, role_id:{}", role_id)
            self._send_enter_game_rsp(conn_id, ErrorCode.CROSS_TOKEN_ERROR)
            return

        def _on_query_remote_login_scene(err_code, result=None):
            if err_code != ErrorCode.OK:
                self._send_enter_game_rsp(conn_id, err_code)
                return
            self._send_enter_game_rsp(conn_id, ErrorCode.OK)
            self._send_kcp_start_info(conn_id)

        future = self.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "Player_RemoteEnterGame", 30, conn_id=conn_id, role_id=role_id)

        future.on_fin += _on_query_remote_login_scene
        future.on_timeout += _on_query_remote_login_scene

        logger.log_info("remote enter game, role_id:{}", role_id)
