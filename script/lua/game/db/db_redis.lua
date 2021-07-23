require("util.logger")

clsRedis = clsObject:Inherit("clsRedis")

function clsRedis:__init__(redisIp, redisPort)
    self.redis = LuaRedis.new(redisIp, redisPort)
    local ret = self.redis:execRedisCmd("hget test a")
    print("redis return ", type(ret), ret)
end
