require("base.object")
require("util.timer")
require("util.const")
require("util.str_util")

local DEFAULT_TIME_OUT = 30

clsFuture = clsObject:Inherit("clsFuture")

function clsFuture:__init__(rpc, rpcId, timeout)
    if timeout < 0 then
        timeout = DEFAULT_TIME_OUT
    end
    self.rpc = rpc
    self.rpcId = rpcId
    self.callback = {}
    self.timeout = timeout
    self.timerId = -1
    self:initTimer()
end

function clsFuture:initTimer()
    self.timerId = timer.add_timer(self.timeout, 1, function(timer_id) self:onTimeout() end)
end

function clsFuture:onTimeout()
    print("clsFuture:onTimeout", #self.callback)
    for _, cb in ipairs(self.callback) do
        cb(ErrorCode.TIME_OUT, nil)
    end
    self.timerId = -1
    self.rpc:removeFuture(self.rpcId)
end

function clsFuture:regCallback(callback)
    table.insert(self.callback, callback)
end

function clsFuture:onRecvResp(msg)
    local res = StrUtil.strToTable(msg.rpc_data)
    for _, cb in ipairs(self.callback) do
        cb(res.errCode, res.result)
    end
    self.rpc:removeFuture(self.rpcId)
end

function clsFuture:Destroy()
    self.rpc = nil
    self.callback = nil
    if self.timerId > 0 then
        timer.remove_timer(self.timerId)
    end
    print("clsFuture:Destroy")
end
