
import PyDb


class DBRedis(object):

    def __init__(self, redis_ip, redis_port):
        self._redis_inst = PyDb.Redis(redis_ip, redis_port)
        assert(self._redis_inst is not None)

    def exec_redis_cmd(self, cmd, *p):
        return self._redis_inst.execRedisCmd(cmd.format(*p))
