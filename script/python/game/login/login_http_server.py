from http_util.http_server import HttpServer
from http_util.http_resp import HttpResp


class LoginHttpServer(HttpServer):

    def __init__(self, service, port):
        self._service = service
        HttpServer.__init__(self, port)

    def on_recv_http_req(self, conn_id, req):
        print("on_recv_http_req----", conn_id, req.method, req.uri, req.headers)
        if req.uri == "/test":
            resp = HttpResp()
            resp.status = 200
            resp.body = "nihao"
            return resp
