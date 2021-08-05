require("base.object")
require("util.logger")
require("base.rpc")
require("base.service_type")
require("base.db_proxy")
require("base.id_mgr")
require("util.const")
require("base.hotfix")

clsServiceBase = clsObject:Inherit("clsServiceBase")

function clsServiceBase:__init__()
    self._serviceMsgHandler = {}
    self._clientMsgHandler = {}
    self.db_proxy = clsDBProxy:New(self)
    self._rpc_mgr = clsRpc:New(self)
    self:_init_id_mgr()
    self:regServiceMsgHandler(MSG_ID_RPC_MSG, self.onRecvRpcMsg)
    self:regServiceMsgHandler(MSG_ID_RPC_MSG_RSP, self.onRecvRpcResp)
    self:regRpcHandler("RpcHotfix", self.rpcHotfix)
    logger.logDebug("clsServiceBase:__init_")
end

function clsServiceBase:_init_id_mgr()
    local redis_ip = Config:getConfigStr("id_redis_ip")
    local redis_port = Config:getConfigInt("id_redis_port")
    if redis_ip == "" and redis_port <= 0 then return end
    IDMgr.connect_redis(redis_ip, redis_port)
end

function clsServiceBase:regServiceMsgHandler(msgId, handler)
    self._serviceMsgHandler[msgId] = handler
end

function clsServiceBase:regClientMsgHandler(msgId, handler)
    self._clientMsgHandler[msgId] = handler
end

function clsServiceBase:regRpcHandler(funcName, func)
    self._rpc_mgr:regRpcHandler(funcName, func)
end

function clsServiceBase:on_recv_service_msg(sender, msgId, msg)
    -- print(self:GetType(), sender, msgId, msg, #msg, pb.tohex(msg))
    logger.logInfo("on_recv_service_msg, sender:%s, msgId:%d", sender, msgId)

    local handler = self._serviceMsgHandler[msgId]
    if handler == nil then
        logger.logError("not found service msg handler, msgId:%d", msgId)
        return
    end

    local sender = parseToServiceAddr(sender)

    local pbMsg = decodeMsg(msgId, msg)
    handler(self, sender, msgId, pbMsg)
end

function clsServiceBase:on_recv_client_msg(connId, msgId, msg)
	--if msgId == 15 then msg = string.sub(msg, 1, 6) end
    -- print("on_recv_client_msg, connId:%d, msgId:%d", connId, msgId, msg, #msg, pb.tohex(msg))
    logger.logInfo("on_recv_client_msg, connId:%d, msgId:%d", connId, msgId)
    local handler = self._clientMsgHandler[msgId]
    if handler == nil then
        logger.logError("not found client msg handler, msgId:%d", msgId)
        return
    end
    local pbMsg = decodeMsg(msgId, msg)
    handler(self, connId, msgId, pbMsg)
end

function clsServiceBase:sendMsgToService(dstAddr, msgId, msg)
    logger.logInfo("sendMsgToService to %s, msgId:%d", StrUtil.tableToStr(dstAddr), msgId)
    local msgName = MSG_ID_TO_NAME[msgId]
    local data = encodeMsg(msgId, msg)
    -- print(dstAddr, msgId, data, string.len(data))
    Service.sendMsgToService(dstAddr, msgId, data, string.len(data))
end

function clsServiceBase:sendMsgToClient(connId, msgId, msg)
    logger.logInfo("sendMsgToClient to %d, msgId:%d", connId, msgId)
    local msgName = MSG_ID_TO_NAME[msgId]
    local data = encodeMsg(msgId, msg)
    -- print(dstAddr, msgId, data, string.len(data))
    Service.sendMsgToClient(connId, msgId, data, string.len(data))
end

function clsServiceBase:onRecvRpcMsg(sender, msgId, msg)
    self._rpc_mgr:onRecvRpcMsg(sender, msg)
end

function clsServiceBase:onRecvRpcResp(sender, msgId, msg)
    self._rpc_mgr:onRecvRpcResp(sender, msg)
end

function clsServiceBase:callRpc(dstAddr, funcName, timeout, args)
    return self._rpc_mgr:callRpc(dstAddr, funcName, timeout, args)
end

function clsServiceBase:rpcHotfix(sender, param)
    Hotfix.hotfix()
    return ErrorCode.OK
end
