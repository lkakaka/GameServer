
class ServiceBase:

    def __init__(self):
        pass

    def on_service_start(self):
        pass

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        print("service on_recv_client_msg error, must override in drived class!!!", conn_id, msg_id, msg_data)

    def on_recv_service_msg(self, sender, msg_id, msg_data):
        print("service on_recv_service_msg error, must override in drived class!!!", sender, msg_id, msg_data)
