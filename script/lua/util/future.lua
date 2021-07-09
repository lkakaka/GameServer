require("base.object")
require("util.timer")
require("util.const")

local DEFAULT_TIME_OUT = 30

clsFuture = clsObject:Inherit("clsFuture")

function clsFuture:__init__(rpc, rpcId, timeout)
    if timeout <= 0 then
        timeout = DEFAULT_TIME_OUT
    end
    self.rpc = rpc
    self.rpcId = rpcId
    self.callback = {}
    self.timeout = timeout
    self.timerId = -1
end

function clsFuture:initTimer()
    if self.timeout <= 0 then return end
    self.timerId = timer.addTimer(self.timeout, 1, function self.onTimeout() end)
end

function clsFuture:onTimeout()
    for i, cb in self.callback do
        cb(ErrorCode.TIME_OUT)
    end
    self.rpc.removeFuture(self.rpcId)
end

function clsFuture:addCallback(callback)
    self.callback[#self.callback + 1] = callback
end

function clsFuture:onRecvResp(msg)
    for i, cb in self.callback do
        cb(ErrorCode.OK, msg.rpc_data)
    end
    self.rpc.removeFuture(self.rpcId)
end

function clsFuture:Destroy()
    self.rpc = nil
    self.callback = nil
    if self.timerId > 0 then
        timer.removeTimer(self.timerId)
    end
    print("clsFuture:Destroy")
end
