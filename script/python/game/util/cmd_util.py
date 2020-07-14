from game.util import logger


class CmdDispatch:
    def __init__(self, name):
        self._name = name
        self._cmd_funcs = {}

    def reg_cmd(self, cmd_id):
        def wrap_func(func):
            def cmd_func(*args, **kwargs):
                return func(*args, **kwargs)

            if cmd_id in self._cmd_funcs:
                logger.log_error("cmd func has existed!!!, name:{}, cmd_id:{}", self._name, cmd_id)
                return self._cmd_funcs[cmd_id]
            self._cmd_funcs[cmd_id] = cmd_func
            # print("reg cmd, cmd_id:{}".format(cmd_id))

            return cmd_func

        return wrap_func

    def reg_cmd_func(self, cmd_id, cmd_func):
        if cmd_id in self._cmd_funcs:
            logger.log_error("reg cmd func has existed!!!, name:{}, cmd_id:{}", self._name, cmd_id)
            return
        self._cmd_funcs[cmd_id] = cmd_func

    def get_cmd_func(self, cmd_id):
        cmd_func = self._cmd_funcs.get(cmd_id, None)
        if cmd_func is None:
            logger.log_error("not found cmd func, name:{}, cmd_id:{}", self._name, cmd_id)

        return cmd_func
