
require("util.str_util")

SERVICE_TYPE_ROUTER = 1
SERVICE_TYPE_GATEWAY = 2
SERVICE_TYPE_LOGIN = 3
SERVICE_TYPE_DB = 4
SERVICE_TYPE_SCENE = 5
SERVICE_TYPE_SCENE_CTRL = 6

SERVER_GROUP_ID = Config:getConfigInt("server_id")
assert(SERVER_GROUP_ID > 0)

LOCAL_SERVICE_DB = { serviceGroup = SERVER_GROUP_ID, serviceType = SERVICE_TYPE_DB, serviceId = 0 }
LOCAL_SERVICE_SCENE_CTRL = { serviceGroup = SERVER_GROUP_ID, serviceType = SERVICE_TYPE_SCENE_CTRL, serviceId = 0 }
LOCAL_SERVICE_LOGIN = { serviceGroup = SERVER_GROUP_ID, serviceType = SERVICE_TYPE_LOGIN, serviceId = 0 }
LOCAL_SERVICE_GATEWAY = { serviceGroup = SERVER_GROUP_ID, serviceType = SERVICE_TYPE_GATEWAY, serviceId = 0 }

function parseToServiceAddr(str)
    local arr = StrUtil.split(str, ".")
    return { serviceGroup = tonumber(arr[1]), serviceType = tonumber(arr[2]), serviceId = tonumber(arr[3]) }
end

function make_scene_ctrl_addr(server_group)
    return { serviceGroup = server_group, serviceType = SERVICE_TYPE_SCENE_CTRL, serviceId = 0 }
end

function make_db_addr(server_group)
    return { serviceGroup = server_group, serviceType = SERVICE_TYPE_DB, serviceId = 0 }
end
