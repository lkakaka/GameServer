

# 全局变量定义
class GlobalVar(object):
    MAX_ROLE_NUM = 6


# 错误码定义
class ErrorCode(object):
    OK = 0

    TIME_OUT = -10000
    DB_ERROR = -10001

    CREATE_PLAYER_ERROR = -20000
    ACCOUNT_IS_LOGINING = -20001
    CONN_INVALID = -20002
    ROLE_COUNT_LIMIT = -20003
    ROLE_NAME_EXIST = -20004
    NOT_FOUND_SCENE = -20005

