from game.util.multi_index_container import MultiIndexContainer
from game.util.multi_index_container import MultiIndexElement
from game.util import logger


class PlayerState:
    UNKNOW = 0      # 未知状态
    LOGINING = 1    # 登录中
    IN_SCENE = 2    # 在场景中
    SWITCHING = 3   # 切换场景中
    REMOTE_SWITCHING = 4,   # 切换到其他服场景中


class PlayerInfo(MultiIndexElement):

    ATTR_ROLE_ID = "role_id"
    ATTR_CONN_ID = "conn_id"

    @staticmethod
    def define_multi_index_attr_names():
        return PlayerInfo.ATTR_ROLE_ID, PlayerInfo.ATTR_CONN_ID

    def __init__(self, role_id, conn_id):
        MultiIndexElement.__init__(self)
        self.role_id = role_id
        self.conn_id = conn_id
        self.state = PlayerState.UNKNOW
        self.scene_uid = 0
        self.scene_id = 0


class PlayerMgr(object):
    def __init__(self):
        self._mi_player_info = MultiIndexContainer(PlayerInfo)

    def add_player(self, role_id, conn_id):
        player_info = PlayerInfo(role_id, conn_id)
        self._mi_player_info.add_elem(player_info)
        return player_info

    def remove_player(self, role_id):
        player_info = self.get_player_info_by_role_id(role_id)
        if player_info is None:
            return
        self._mi_player_info.remove_elem(player_info)

    def get_player_info_by_role_id(self, role_id):
        return self._mi_player_info.get_one_elem(PlayerInfo.ATTR_ROLE_ID, role_id)

    def get_player_info_by_conn_id(self, conn_id):
        return self._mi_player_info.get_one_elem(PlayerInfo.ATTR_CONN_ID, conn_id)

    def switch_state(self, role_id, state, **kwargs):
        player_info = self.get_player_info_by_role_id(role_id)
        if player_info is None:
            logger.log_error("not found player info, role_id:{0}", role_id)
            return
        player_info.state = state
        for attr_name in ("scene_uid", "scene_id"):
            val = kwargs.get(attr_name, None)
            if val is None:
                continue
            player_info.__dict__[attr_name] = val

