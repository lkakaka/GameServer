require("base.object")
require("util.logger")

clsRpc = clsObject:Inherit("clsRpc")

function clsRpc:__init__()
    self.cur_rpc_id = 0
    self._futures = {}
end

function clsRpc:allocRpcId()
    self.cur_rpc_id += 1
    return self.cur_rpc_id
end

function clsRpc:onRecvRpcResp(msg)
    local future = self._futures[msg.rpc_id]
    if future == nil then
        logger.log_error("not found rpc future, rpc_id:%d", msg.rpc_id)
        return
    end
    future.onRecvResp(msg)
end

function clsRpc:callRpc(dstService, funcName, timeout, args)
    local rpcId = allocRpcId()
    local future = future:New(self, rpcId, timeout)
    self._futures[rpcId] = future
    return future
end

function clsRpc:removeFuture(rpcId)
    local future = self._futures[rpcId]
    if future == nil then
        logger.log_error("not found rpc future, rpc_id:%d", rpcId)
        return
    end
    self._futures[rpcId] = nil
    future.Destory()
end
