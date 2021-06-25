import Config


class ServiceType:
    SERVICE_TYPE_UNKNOW = -1
    SERVICE_TYPE_START = 0
    # 服务类型定义
    SERVICE_TYPE_ROUTER = 1
    SERVICE_TYPE_GATEWAY = 2
    SERVICE_TYPE_LOGIN = 3
    SERVICE_TYPE_DB = 4
    SERVICE_TYPE_SCENE = 5
    SERVICE_TYPE_SCENE_CTRL = 6
    # 服务类型定义结束(增加类型需要修改SERVICE_TYPE_END)
    SERVICE_TYPE_END = 7


class ServiceAddr(object):

    def __init__(self, service_group, service_type, service_id):
        self.service_group = service_group
        self.service_type = service_type
        self.service_id = service_id

    def __str__(self):
        return "{0}.{1}.{2}".format(self.service_group, self.service_type, self.service_id)

    @staticmethod
    def parse_service_addr(addr):
        arr = addr.split(".")
        return ServiceAddr(int(arr[0]), int(arr[1]), int(arr[2]))


LOCAL_SERVICE_GROUP = Config.getConfigInt("server_id")
LOCAL_DB_SERVICE_ADDR = ServiceAddr(LOCAL_SERVICE_GROUP, ServiceType.SERVICE_TYPE_DB, 0)
LOCAL_LOGIN_SERVICE_ADDR = ServiceAddr(LOCAL_SERVICE_GROUP, ServiceType.SERVICE_TYPE_LOGIN, 0)
LOCAL_SCENE_CTRL_SERVICE_ADDR = ServiceAddr(LOCAL_SERVICE_GROUP, ServiceType.SERVICE_TYPE_SCENE_CTRL, 0)
LOCAL_GATEWAY_SERVICE_ADDR = ServiceAddr(LOCAL_SERVICE_GROUP, ServiceType.SERVICE_TYPE_GATEWAY, 0)
