require("util.const")
require("game.db.tbl_defs")

IDMgr = {}

IDMgr._redis = nil

IDMgr.REDIS_KEY_ROLE = "player"
IDMgr.REDIS_KEY_ITEM = "item"

IDMgr._id_alloctor_names = {
    IDMgr.REDIS_KEY_ROLE,
    IDMgr.REDIS_KEY_ITEM,
}

-- ID共64位，第64位为符号位，41-63为服务器ID，1-40位为自增ID
IDMgr.ID_BIT_COUNT = 40

function IDMgr.connect_redis(redis_ip, redis_port)
    if IDMgr._redis ~= nil then return end
    IDMgr._redis = LuaRedis.new(redis_ip, redis_port)
    logger.logInfo("connect id redis, ip:%s, port:%d", redis_ip, redis_port)
end

function IDMgr._make_uid(id)
    return (SERVER_GROUP_ID << IDMgr.ID_BIT_COUNT) + id
end

function IDMgr.get_server_id_by_uid(uid)
    return uid >> IDMgr.ID_BIT_COUNT
end

function IDMgr.alloc_role_id(count)
    local role_id = IDMgr._redis:execRedisCmd(string.format("HINCRBY %s %s %d", RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ROLE, count))
    return IDMgr._make_uid(tonumber(role_id))
end

function IDMgr.alloc_item_uid(count)
    local item_uid = IDMgr._redis:execRedisCmd(string.format("HINCRBY %s %s %d", RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ITEM, count))
    print("alloc_item_uid", item_uid, type(item_uid))
    return IDMgr._make_uid(tonumber(item_uid))
end
