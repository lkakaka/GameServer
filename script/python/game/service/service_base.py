
import Game
from proto.pb_message import Message
# import game.util.cmd_util
import logger


class ServiceBase:

    # s_cmd = game.util.cmd_util.CmdDispatch("s_service")
    # c_cmd = game.util.cmd_util.CmdDispatch("c_service")

    def __init__(self, s_cmd, c_cmd):
        self._service_obj = Game.Service()
        self._s_cmd = s_cmd
        self._c_cmd = c_cmd
        print("service obj = ", self._service_obj)

    def on_service_start(self):
        pass

    # def on_recv_client_msg(self, conn_id, msg_id, msg_data):
    #     logger.logError("$service on_recv_client_msg error, must override in drived class!!!", conn_id, msg_id, msg_data)

    # def on_recv_service_msg(self, sender, msg_id, msg_data):
    #     logger.logError("service on_recv_service_msg error, must override in drived class!!!", sender, msg_id, msg_data)

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        logger.logInfo("$recv client msg, conn_id:{}, msg_id:{}", conn_id, msg_id)
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

    def send_msg_to_client(self, conn_id, msg_id, msg):
        msg_dat = msg.SerializeToString()
        self._service_obj.sendMsgToClient(conn_id, msg_id, msg_dat)

    def send_msg_to_service(self, dst_srv, msg_id, msg):
        msg_dat = msg.SerializeToString()
        self._service_obj.sendMsgToService(dst_srv, msg_id, msg_dat)
