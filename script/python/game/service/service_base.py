
import Game
from proto.pb_message import Message
# import game.util.cmd_util
import logger
from game.util.rpc import RpcMgr


class ServiceBase:

    # s_cmd = game.util.cmd_util.CmdDispatch("s_service")
    # c_cmd = game.util.cmd_util.CmdDispatch("c_service")

    def __init__(self, s_cmd, c_cmd, rpc_proc=None):
        self._service_obj = Game.Service()
        self._s_cmd = s_cmd
        self._c_cmd = c_cmd
        self._rpc_proc = rpc_proc
        self._rpc_mgr = RpcMgr(self)
        s_cmd.reg_cmd_func(Message.MSG_ID_RPC_MSG_RSP, ServiceBase._on_recv_rpc_rsp_msg)
        s_cmd.reg_cmd_func(Message.MSG_ID_RPC_MSG, ServiceBase._on_recv_rpc_msg)
        print("service obj = ", self._service_obj)

    def on_service_start(self):
        pass

    # def on_recv_client_msg(self, conn_id, msg_id, msg_data):
    #     logger.logError("$service on_recv_client_msg error, must override in drived class!!!", conn_id, msg_id, msg_data)

    # def on_recv_service_msg(self, sender, msg_id, msg_data):
    #     logger.logError("service on_recv_service_msg error, must override in drived class!!!", sender, msg_id, msg_data)

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        logger.logInfo("$recv client msg, conn_id:{}, msg_id:{}", conn_id, msg_id)
        if self._c_cmd is None:
            logger.logInfo("$on_recv_client_msg error, not set c_cmd, msgId:{}", msg_id)
            return
        func = self._c_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.logInfo("$on_recv_client_msg error, not found cmd func, msgId:{}", msg_id)
            return
        msg = Message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func(self, conn_id, msg_id, msg)

    def on_recv_service_msg(self, sender, msg_id, msg_data):
        logger.logInfo("$recv service msg, sender:{}, msg_id:{}", sender, msg_id)
        func = self._s_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.logInfo("$on_recv_service_msg error, not found cmd func, msgId:{}", msg_id)
            return
        msg = Message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func(self, sender, msg_id, msg)

    def _on_recv_rpc_msg(self, sender, msg_id, msg):
        if self._rpc_proc is None:
            logger.logInfo("$on_recv_rpc_msg error, not set rpc proc func, rpc_func:{}", msg.rpc_func)
            return
        func = self._rpc_proc.get_cmd_func(msg.rpc_func)
        if func is None:
            logger.logInfo("$on_recv_rpc_msg error, not found rpc proc func, rpc_func:{}", msg.rpc_func)
            return
        dict_param = eval(msg.rpc_param)
        print(type(dict_param), dict_param)
        rpc_data = func(self, sender, dict_param)
        rpc_rsp_msg = Message.create_msg_by_id(Message.MSG_ID_RPC_MSG_RSP)
        rpc_rsp_msg.rpc_id = msg.rpc_id
        rpc_rsp_msg.rpc_data = rpc_data
        self.send_msg_to_service(sender, Message.MSG_ID_RPC_MSG_RSP, rpc_rsp_msg)

    def _on_recv_rpc_rsp_msg(self, sender, msg_id, msg):
        self._rpc_mgr.on_recv_rpc_rsp_msg(sender, msg)

    def send_msg_to_client(self, conn_id, msg_id, msg):
        msg_dat = msg.SerializeToString()
        self._service_obj.sendMsgToClient(conn_id, msg_id, msg_dat)

    def send_msg_to_service(self, dst_srv, msg_id, msg):
        msg_dat = msg.SerializeToString()
        self._service_obj.sendMsgToService(dst_srv, msg_id, msg_dat)

    def rpc_call(self, dst_srv, func_name, func_args, timeout=RpcMgr.DEFAULT_TIME_OUT, **kwargs):
        return self._rpc_mgr.rpc_call(dst_srv, func_name, func_args, timeout, **kwargs)
