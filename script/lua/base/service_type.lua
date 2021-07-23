
require("util.str_util")

SERVICE_TYPE_ROUTER = 1
SERVICE_TYPE_GATEWAY = 2
SERVICE_TYPE_LOGIN = 3
SERVICE_TYPE_DB = 4
SERVICE_TYPE_SCENE = 5
SERVICE_TYPE_SCENE_CTRL = 6

LOCAL_SERVICE_DB = { serviceGroup = 101, serviceType = SERVICE_TYPE_DB, serviceId = 0 }
LOCAL_SERVICE_SCENE_CTRL = { serviceGroup = 101, serviceType = SERVICE_TYPE_SCENE_CTRL, serviceId = 0 }
LOCAL_SERVICE_LOGIN = { serviceGroup = 101, serviceType = SERVICE_TYPE_LOGIN, serviceId = 0 }
LOCAL_SERVICE_GATEWAY = { serviceGroup = 101, serviceType = SERVICE_TYPE_GATEWAY, serviceId = 0 }

function parseToServiceAddr(str)
    local arr = StrUtil.split(str, ".")
    -- print(arr[1], arr[2], arr[3])
    return { serviceGroup = tonumber(arr[1]), serviceType = tonumber(arr[2]), serviceId = tonumber(arr[3]) }
end

-- parseToServiceAddr("10.1.1")
