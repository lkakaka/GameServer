
import PyDb


class DBRedis(object):

    def __init__(self):
        self._redis_inst = PyDb.Redis()
        assert(self._redis_inst is not None)

    def exec_redis_cmd(self, cmd, *p):
        return self._redis_inst.execRedisCmd(cmd.format(*p))
