

# 全局变量定义
class GlobalVar(object):
    MAX_ROLE_NUM = 6


class RedisKey(object):
    ID_ALLOCATOR = "_id_allocator"


# 错误码定义
class ErrorCode(object):
    OK = 0

    TIME_OUT = -10000
    DB_ERROR = -10001
    ILLEGAL_PARAM = -10002

    CREATE_PLAYER_ERROR = -20000
    ACCOUNT_IS_LOGINING = -20001
    CONN_INVALID = -20002
    ROLE_COUNT_LIMIT = -20003
    ROLE_NAME_EXIST = -20004
    NOT_FOUND_SCENE = -20005

    ITEM_NOT_EXIST = -20100
    ITEM_NOT_ENOUGH = -20101
    BAG_SPACE_NOT_ENOUGH = -20102

