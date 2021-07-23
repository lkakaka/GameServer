require("util.const")
require("game.db.tbl_defs")

IDMgr = {}

IDMgr._redis = nil

IDMgr.REDIS_KEY_ROLE = TblDefs.player.tb_name
IDMgr.REDIS_KEY_ITEM = TblDefs.item.tb_name

function IDMgr.connect_redis(redis_ip, redis_port)
    if IDMgr._redis ~= nil then return end
    IDMgr._redis = LuaRedis.new(redis_ip, redis_port)
    logger.logInfo("connect id redis, ip:%s, port:%d", redis_ip, redis_port)
end

function IDMgr.alloc_role_id(count)
    local role_id = IDMgr._redis:execRedisCmd(string.format("HINCRBY {} {} {}", RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ROLE, count))
    return tonumber(role_id)
end

function IDMgr.alloc_item_uid(count)
    local item_uid = IDMgr._redis:execRedisCmd(string.format("HINCRBY {} {} {}", RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ITEM, count))
    print("alloc_item_uid", item_uid, type(item_uid))
    return tonumber(item_uid)
end
