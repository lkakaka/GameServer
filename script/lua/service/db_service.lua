require("service.service_base")
require("util.logger")
require("game.db.db_handler")
require("util.const")

clsDBService = clsServiceBase:Inherit("clsDBService")

function clsDBService:__init__()
    Super(clsDBService).__init__(self)
    print("clsDBService:__init__")
    self.db_hander = clsDBHandler:New("save", "127.0.0.1", 6379)
    self:initRpcHandler()
end

function clsDBService:initRpcHandler()
    self:regRpcHandler("RpcLoadDB", self.rpcLoad);
end

function clsDBService:rpcLoad(sender, param)
    local result = self.db_hander:load(param)
    return ErrorCode.OK, result
end

function clsDBService:rpcInsert(sender, param)
    local result = self.db_hander:insert(param)
    return ErrorCode.OK, result
end

function clsDBService:rpcUpdate(sender, param)
    local result = self.db_hander:update(param)
    return ErrorCode.OK, result
end

function clsDBService:rpcDelete(sender, param)
    local result = self.db_hander:delete(param)
    return ErrorCode.OK, result
end
