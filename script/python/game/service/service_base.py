
import Game

class ServiceBase:

    def __init__(self):
        self._service_obj = Game.Service()
        print("service obj = ", self._service_obj)

    def on_service_start(self):
        pass

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        print("service on_recv_client_msg error, must override in drived class!!!", conn_id, msg_id, msg_data)

    def on_recv_service_msg(self, sender, msg_id, msg_data):
        print("service on_recv_service_msg error, must override in drived class!!!", sender, msg_id, msg_data)

    def send_msg_to_client(self, conn_id, msg_id, msg):
        msg_dat = msg.SerializeToString()
        self._service_obj.sendMsgToClient(conn_id, msg_id, msg_dat)

    def send_msg_to_service(self, dst_srv, msg_id, msg):
        msg_dat = msg.SerializeToString()
        self._service_obj.sendMsgToService(dst_srv, msg_id, msg_dat)
