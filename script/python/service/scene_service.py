
import logger
from proto.message import message
import Game


class SceneService:

    def __init__(self):
        pass

    def on_service_start(self):
        logger.logInfo("$Scene Service Start!!")

    def on_recv_msg(self, sender, msg):
        logger.logInfo("$DBService on_recv_msg!!!")

    def c_call(self):
        print("c call----------", self)

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        print("scene on_recv_client_msg---", conn_id, msg_id, msg_data)
        msg = message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        if msg_id == message.MSG_ID_LOGIN:
            print("recv login---", msg.account, msg.pwd)
            rsp_msg = message.create_msg_by_id(message.MSG_ID_LOGINRSP)
            rsp_msg.account = "test"
            rsp_msg.user_id = 1
            rsp_str = rsp_msg.SerializeToString()
            print("login rsp===", rsp_str)
            Game.sendMessage(conn_id, message.MSG_ID_LOGINRSP, rsp_str)
        elif msg_id == message.MSG_ID_TEST:
            rsp_msg = message.create_msg_by_id(message.MSG_ID_TEST)
            rsp_msg.id = 10
            rsp_msg.msg = "hello"
            rsp_str = rsp_msg.SerializeToString()
            Game.sendMessage(conn_id, message.MSG_ID_TEST, rsp_str)

    def on_recv_service_msg(self, sender, msgId, msg):
        print("scene on_recv_service_msg---", self, sender, msgId, msg)