import weakref

import game.util.logger
import game.util.cmd_util


class GMMgr(object):

    _gm_cmd = game.util.cmd_util.CmdDispatch("player_gm_cmd")

    def __init__(self, player):
        self._weak_player_ref = weakref.ref(player)

    @property
    def player(self):
        return self._weak_player_ref()

    def on_recv_gm_cmd(self, cmd, args):
        func = GMMgr._gm_cmd.get_cmd_func(cmd)
        if func is None:
            game.util.logger.log_error("player on_recv_gm_cmd error, not found cmd func, cmd:{}", cmd)
            return "not define cmd"
        return func(self, args)

    @_gm_cmd.reg_cmd("add_item")
    def _gm_add_item(self, args):
        arg_list = args.split(",")
        if len(arg_list) < 2:
            return "arg num error"
        item_id = int(arg_list[0])
        item_count = int(arg_list[1])
        self.player._item_mgr.add_item(((item_id, item_count),))
        return "ok"

    @_gm_cmd.reg_cmd("dump_item")
    def _gm_add_item(self, args):
        return self.player._item_mgr.dump_items()

    @_gm_cmd.reg_cmd("hotfix")
    def _gm_hotfix(self, args):
        if not args:
            import hotfix.hotfix
            hotfix.hotfix.start_hotfix()
        else:
            self.player.game_scene.service.rpc_call(args, "HotFix")
        return "ok"
