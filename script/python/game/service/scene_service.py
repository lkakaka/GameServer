
import logger
from proto.message import message
import Game
from game.service.service_base import ServiceBase


class SceneService(ServiceBase):

    def __init__(self):
        pass

    def on_service_start(self):
        logger.logInfo("$Scene Service Start!!")

    def on_recv_msg(self, sender, msg):
        logger.logInfo("$DBService on_recv_msg!!!")

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        logger.logInfo("$recv client msg, conn_id:{}, msg_id:{}", conn_id, msg_id)
        msg = message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        if msg_id == message.MSG_ID_LOGIN_REQ:
            print("recv login---", msg.account, msg.pwd)
            msg.conn_id = conn_id
            new_msg_data = msg.SerializeToString()
            Game.sendMsgToService("db", msg_id, new_msg_data)
        elif msg_id == message.MSG_ID_TEST_REQ:
            rsp_msg = message.create_msg_by_id(message.MSG_ID_TEST_REQ)
            rsp_msg.id = 10
            rsp_msg.msg = "hello"
            rsp_str = rsp_msg.SerializeToString()
            Game.sendMsgToClient(conn_id, message.MSG_ID_TEST_REQ, rsp_str)

    def on_recv_service_msg(self, sender, msg_id, msg_data):
        logger.logInfo("$recv service msg, sender:{}, msg_id:{}", sender, msg_id)
        msg = message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        if msg_id == message.MSG_ID_LOGIN_RSP:
            conn_id = msg.conn_id
            msg.conn_id = 0
            Game.sendMsgToClient(conn_id, message.MSG_ID_LOGIN_RSP, msg_data)
