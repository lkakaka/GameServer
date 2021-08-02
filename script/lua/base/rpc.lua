require("base.object")
require("util.logger")
require("proto.pb_message")
require("base.future")
require("util.str_util")

clsRpc = clsObject:Inherit("clsRpc")

function clsRpc:__init__(service)
    self._service = service
    self.cur_rpc_id = 0
    self._futures = {}
    self.rpc_func = {}

    -- self._service:regMsgHandler(MSG_ID_RPC_MSG_RSP, function(sender, msgId, msg) self.onRecvRpcResp(sender, msgId, msg) end)
end

function clsRpc:allocRpcId()
    self.cur_rpc_id = self.cur_rpc_id + 1
    return self.cur_rpc_id
end

function clsRpc:regRpcHandler(funcName, func)
    if self.rpc_func[funcName] ~= nil then
        logger.logError("repeat reg rpc hander, %s", funcName)
    end
    self.rpc_func[funcName] = func
end

function clsRpc:onRecvRpcMsg(sender, msg)
    local rpcFunc = self.rpc_func[msg.rpc_func]
    if rpcFunc == nil then
        logger.logError("not found rpc proc func :%s", msg.rpc_func)
        return
    end
    logger.logInfo("recv rpc %s", msg.rpc_func)
    local param = StrUtil.strToTable(msg.rpc_param)
    local errCode, result = rpcFunc(self._service, sender, param)

    local resp = {
        rpc_id = msg.rpc_id,
        rpc_data = StrUtil.tableToStr({ errCode = errCode, result = result}),
    }
    self._service:sendMsgToService(sender, MSG_ID_RPC_MSG_RSP, resp)
end

function clsRpc:onRecvRpcResp(sender, msg)
    local future = self._futures[msg.rpc_id]
    if future == nil then
        logger.logError("not found rpc future, rpc_id:%d", msg.rpc_id)
        return
    end
    future:onRecvResp(msg)
end

function clsRpc:callRpc(dstService, funcName, timeout, args)
    local rpcId = self:allocRpcId()
    local future = clsFuture:New(self, rpcId, timeout)
    self._futures[rpcId] = future
    local rpcMsg = {
        rpc_id = rpcId,
        rpc_func = funcName,
        rpc_param = StrUtil.tableToStr(args),
    }
    self._service:sendMsgToService(dstService, MSG_ID_RPC_MSG, rpcMsg)
    return future
end

function clsRpc:removeFuture(rpcId)
    local future = self._futures[rpcId]
    if future == nil then
        logger.logError("not found rpc future, rpc_id:%d", rpcId)
        return
    end
    self._futures[rpcId] = nil
    future:Destroy()
end
