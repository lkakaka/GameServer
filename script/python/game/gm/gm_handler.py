
import game.util.cmd_util
from game.service.service_addr import ServiceAddr
from game.service.service_addr import LOCAL_SERVICE_GROUP
import Crypt


class GMParam(object):
    role_id = 0
    player = None
    args = None


class GMHandler(object):
    _gm_cmd = game.util.cmd_util.CmdDispatch("gm_cmd")

    def __init__(self, service):
        self.service = service

    def handle_gm_cmd(self, cmd, param):
        func = GMHandler._gm_cmd.get_cmd_func(cmd)
        if func is None:
            game.util.logger.log_error("not found gm handler func, cmd:{}", cmd)
            return "not define cmd"
        return func(self, param)

    @_gm_cmd.reg_cmd("add_item")
    def _gm_add_item(self, param):
        arg_list = param.args.split(",")
        if len(arg_list) < 2:
            return "arg num error"
        item_id = int(arg_list[0])
        item_count = int(arg_list[1])
        param.player._item_mgr.add_item(((item_id, item_count),))
        return "ok"

    @_gm_cmd.reg_cmd("dump_item")
    def _gm_add_item(self, param):
        return param.player._item_mgr.dump_items()

    @_gm_cmd.reg_cmd("hotfix")
    def _gm_hotfix(self, param):
        if not param.args:
            import hotfix.hotfix
            hotfix.hotfix.start_hotfix()
            return "ok"
        arg_list = param.args.split(",")
        service_group = int(arg_list[0])
        service_type = int(arg_list[1])
        service_id = int(arg_list[2])
        self.service.rpc_call(ServiceAddr(service_group, service_type, service_id), "HotFix")
        return "ok"

    @_gm_cmd.reg_cmd("goto_scene")
    def _goto_scene(self, param):
        arg_list = param.args.split(",")
        scene_id = int(arg_list[0])
        server_id = LOCAL_SERVICE_GROUP
        if len(arg_list) >= 2:
            server_id = int(arg_list[1])
        param.player.try_switch_scene(server_id, scene_id)
        return "ok"

    @_gm_cmd.reg_cmd("gen_id")
    def _gen_disorder_id(self, param):
        arg_list = param.args.split(",")
        origin_id = int(arg_list[0])
        disorder_id = Crypt.genDisorderId(origin_id)
        game.util.logger.log_info("origin_id: {0} disorder_id: {1}", origin_id, disorder_id)
        return "ok"
