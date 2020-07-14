
import game.db.db_redis
from game.db.tbl.tbl_player import TblPlayer
from game.db.tbl.tbl_item import TblItem
from game.util.const import RedisKey
from game.util import logger


class IDMgr(object):

    _redis = None

    REDIS_KEY_ROLE = TblPlayer.tb_name
    REDIS_KEY_ITEM = TblItem.tb_name

    def __init__(self):
        pass

    @staticmethod
    def connect_redis(redis_ip, redis_port):
        if IDMgr._redis is not None:
            return
        IDMgr._redis = game.db.db_redis.DBRedis(redis_ip, redis_port)
        logger.log_info("connect id redis, ip:{}, port:{}".format(redis_ip, redis_port))

    @staticmethod
    def alloc_role_id():
        role_id = IDMgr._redis.exec_redis_cmd("HINCRBY {} {} 1".format(RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ROLE))
        return int(role_id)

    @staticmethod
    def alloc_item_uid():
        item_uid = IDMgr._redis.exec_redis_cmd("HINCRBY {} {} 1".format(RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ITEM))
        print("alloc_item_uid", item_uid, type(item_uid))
        return int(item_uid)
