
import game.db.db_redis
from game.db.tbl.tbl_player import TblPlayer
from game.db.tbl.tbl_item import TblItem
from game.util.const import RedisKey
from game.util import logger
from game.service.service_addr import LOCAL_SERVICE_GROUP


class IDMgr(object):

    _redis = None

    REDIS_KEY_ROLE = TblPlayer.tb_name
    REDIS_KEY_ITEM = TblItem.tb_name

    # ID共64位，第64位为符号位，41 - 63为服务器ID，1 - 40位为自增ID
    ID_BIT_COUNT = 40

    def __init__(self):
        pass

    @staticmethod
    def connect_redis(redis_ip, redis_port):
        if IDMgr._redis is not None:
            return
        IDMgr._redis = game.db.db_redis.DBRedis(redis_ip, redis_port)
        logger.log_info("connect id redis, ip:{}, port:{}".format(redis_ip, redis_port))

    @staticmethod
    def get_server_id_by_uid(uid):
        return uid >> IDMgr.ID_BIT_COUNT

    @staticmethod
    def get_server_start_uid():
        return LOCAL_SERVICE_GROUP << IDMgr.ID_BIT_COUNT

    @staticmethod
    def alloc_role_id(count=1):
        role_id = IDMgr._redis.exec_redis_cmd("HINCRBY {} {} {}".format(RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ROLE, count))
        return int(role_id)

    @staticmethod
    def alloc_item_uid(count=1):
        item_uid = IDMgr._redis.exec_redis_cmd("HINCRBY {} {} {}".format(RedisKey.ID_ALLOCATOR, IDMgr.REDIS_KEY_ITEM, count))
        print("alloc_item_uid", item_uid, type(item_uid))
        return int(item_uid)
