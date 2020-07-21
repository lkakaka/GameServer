
import Http
import game.util.logger


class HttpServer(object):

    def __init__(self, server_port):
        # print(server_ip, server_port)
        self._http_server = Http.HttpServer(server_port, self)
        assert(self._http_server is not None)

    def on_recv_http_req(self, conn_id, req):
        game.util.logger.log_info("recv http req")
        print(conn_id, req)

    def send_http_resp(self, conn_id, resp):
        self._http_server.sendHttpResp(conn_id, resp)
