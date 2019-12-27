import logger


class DBService:

    def __init__(self):
        pass

    def on_service_start(self):
        logger.logInfo("$DBService start!!!")

    def on_recv_msg(self, sender, msg):
        logger.logInfo("$DBService on_recv_msg!!!")


