
class ServiceBase:

    def __init__(self):
        pass

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        print("scene on_recv_client_msg error, override", conn_id, msg_id, msg_data)

    def on_recv_service_msg(self, sender, msgId, msg):
        print("scene on_recv_service_msg---", self, sender, msgId, msg)
