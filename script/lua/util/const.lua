
ErrorCode = {
    OK = 0,

    TIME_OUT = -10000,
    DB_ERROR = -10001,
    ILLEGAL_PARAM = -10002,
    GM_CMD_RSP_DELAY = -10003,

    CREATE_PLAYER_ERROR = -20000,
    ACCOUNT_IS_LOGINING = -20001,
    CONN_INVALID = -20002,
    ROLE_COUNT_LIMIT = -20003,
    ROLE_NAME_EXIST = -20004,
    NOT_FOUND_SCENE = -20005,

    ITEM_NOT_EXIST = -20100,
    ITEM_NOT_ENOUGH = -20101,
    BAG_SPACE_NOT_ENOUGH = -20102,
}

GlobalVar = {
    MAX_ROLE_NUM = 6,
}

RedisKey = {
    ID_ALLOCATOR = "_id_allocator",
}

TokenPrefix = {
    KCP = "KCPToken_$gs20210915@",
}
