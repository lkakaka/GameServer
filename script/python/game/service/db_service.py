import logger
from game.service.service_base import ServiceBase
from proto.message import message
import Game


class DBService(ServiceBase):

    def __init__(self):
        pass

    def on_service_start(self):
        logger.logInfo("$DBService start!!!")

    def on_recv_service_msg(self, sender, msg_id, msg_data):
        logger.logInfo("$DBService on_recv_msg, sender:{}, msg_id:{}", sender, msg_id)
        msg = message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        if msg_id == message.MSG_ID_LOGIN_REQ:
            rsp_msg = message.create_msg_by_id(message.MSG_ID_LOGIN_RSP)
            rsp_msg.account = "test"
            rsp_msg.user_id = 1
            rsp_msg.conn_id = msg.conn_id
            rsp_msg.err_code = 1
            rsp_str = rsp_msg.SerializeToString()
            Game.sendMsgToService("scene", message.MSG_ID_LOGIN_RSP, rsp_str)


