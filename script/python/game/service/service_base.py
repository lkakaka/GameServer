# -*- encoding:utf-8 -*-

import Game
import Config
from proto.pb_message import Message
from game.util import logger
from game.util.rpc import RpcMgr
from game.util.db_proxy import DBProxy
import game.util.cmd_util

# import asyncio


class ServiceBase:

    rpc_proc = game.util.cmd_util.CmdDispatch("rpc_base")

    def __init__(self, s_cmd, c_cmd, rpc_proc=None):
        self._service_obj = Game.Service()
        self.db_proxy = DBProxy(self)
        self._s_cmd = s_cmd
        self._c_cmd = c_cmd
        if rpc_proc is not None:
            self._rpc_proc = rpc_proc
        else:
            self._rpc_proc = ServiceBase.rpc_proc
        self._rpc_proc.reg_cmd_func("HotFix", ServiceBase._on_recv_hotfix)
        self._rpc_mgr = RpcMgr(self)
        s_cmd.reg_cmd_func(Message.MSG_ID_RPC_MSG_RSP, ServiceBase._on_recv_rpc_rsp_msg)
        s_cmd.reg_cmd_func(Message.MSG_ID_RPC_MSG, ServiceBase._on_recv_rpc_msg)
        print("service obj = ", self._service_obj)

    def on_service_start(self):
        self._init_id_mgr()

    def _init_id_mgr(self):
        redis_ip = Config.getConfigStr("id_redis_ip")
        redis_port = Config.getConfigInt("id_redis_port")
        if not redis_ip or not redis_port:
            return
        import game.util.id_mgr
        game.util.id_mgr.IDMgr.connect_redis(redis_ip, redis_port)

    # def start_asyncio_loop(self):
    #     loop = asyncio.get_event_loop()
    #
    #     # Schedule a call to hello_world()
    #     # loop.call_soon(hello_world, loop)
    #
    #     # Blocking call interrupted by loop.stop()
    #     try:
    #         print("loop start")
    #         loop.run_forever()
    #     finally:
    #         loop.close()
    #         print("loop end")

    # def on_recv_client_msg(self, conn_id, msg_id, msg_data):
    #     logger.logError("$service on_recv_client_msg error, must override in drived class!!!", conn_id, msg_id, msg_data)

    # def on_recv_service_msg(self, sender, msg_id, msg_data):
    #     logger.logError("service on_recv_service_msg error, must override in drived class!!!", sender, msg_id, msg_data)

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        logger.log_info("recv client msg, conn_id:{}, msg_id:{}", conn_id, msg_id)
        if self._c_cmd is None:
            logger.log_info("on_recv_client_msg error, not set c_cmd, msgId:{}", msg_id)
            return
        func = self._c_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.log_info("on_recv_client_msg error, not found cmd func, msgId:{}", msg_id)
            return
        msg = Message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func(self, conn_id, msg_id, msg)

    def on_recv_service_msg(self, sender, msg_id, msg_data):
        logger.log_info("recv service msg, sender:{}, msg_id:{}", sender, msg_id)
        func = self._s_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.log_info("on_recv_service_msg error, not found cmd func, msgId:{}", msg_id)
            return
        msg = Message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func(self, sender, msg_id, msg)

    def _on_recv_rpc_msg(self, sender, msg_id, msg):
        if self._rpc_proc is None:
            logger.log_info("on_recv_rpc_msg error, not set rpc proc func, rpc_func:{}", msg.rpc_func)
            return
        func = self._rpc_proc.get_cmd_func(msg.rpc_func)
        if func is None:
            logger.log_info("on_recv_rpc_msg error, not found rpc proc func, rpc_func:{}", msg.rpc_func)
            return
        dict_param = eval(msg.rpc_param)
        rpc_data = func(self, sender, **dict_param)
        rpc_rsp_msg = Message.create_msg_by_id(Message.MSG_ID_RPC_MSG_RSP)
        rpc_rsp_msg.rpc_id = msg.rpc_id
        if rpc_data is not None:
            if type(rpc_data) in (tuple, list):
                rpc_rsp_msg.rpc_data = repr(rpc_data)
            else:
                rpc_rsp_msg.rpc_data = repr((rpc_data,))
        self.send_msg_to_service(sender, rpc_rsp_msg)

    def _on_recv_rpc_rsp_msg(self, sender, msg_id, msg):
        self._rpc_mgr.on_recv_rpc_rsp_msg(sender, msg.rpc_id, msg.rpc_data)

    def _on_recv_hotfix(self, sender):
        import hotfix.hotfix
        hotfix.hotfix.start_hotfix()

    def send_msg_to_client(self, conn_id, msg):
        msg_dat = msg.SerializeToString()
        msg_id = Message.get_msg_id(msg)
        self._service_obj.sendMsgToClient(conn_id, msg_id, msg_dat)

    def send_msg_to_service(self, dst_srv, msg):
        msg_dat = msg.SerializeToString()
        msg_id = Message.get_msg_id(msg)
        self._service_obj.sendMsgToService(dst_srv, msg_id, msg_dat)

    def rpc_call(self, dst_srv, func_name, timeout=RpcMgr.DEFAULT_TIME_OUT, **kwargs):
        return self._rpc_mgr.rpc_call(dst_srv, func_name, timeout, **kwargs)
