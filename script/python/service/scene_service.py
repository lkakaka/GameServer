
import logger


class SceneService:

    def __init__(self):
        pass

    def on_service_start(self):
        logger.logInfo("$Scene Service Start!!")

    def on_recv_msg(self, sender, msg):
        logger.logInfo("$DBService on_recv_msg!!!")

