require("base.object")
require("util.logger")
require("util.str_util")

clsHttpServer = clsObject:Inherit("clsHttpServer")

function clsHttpServer:__init__(port)
    self._http_server = HttpServer.createHttpServer(port, self)
    logger.logInfo("create http server, %d", port)
end

function clsHttpServer:on_recv_http_req(conn_id, req)
    logger.logInfo("*****recv http req, %d, %s", conn_id, StrUtil.tableToStr(req))
    return {status=200, body="ok"}
end