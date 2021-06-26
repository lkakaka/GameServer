from game.util import logger
from game.util.multi_index_container import MultiIndexContainer
from game.util.multi_index_container import MultiIndexElement


class _Scene(MultiIndexElement):
    MULTI_INDEX_ATTR_SCENE_ID = "scene_id"
    MULTI_INDEX_ATTR_SCENE_UID = "scene_uid"
    MULTI_INDEX_ATTR_SERVICE_ADDR = "service_addr"

    @staticmethod
    def define_multi_index_attr_names():
        return (_Scene.MULTI_INDEX_ATTR_SCENE_ID,
                _Scene.MULTI_INDEX_ATTR_SCENE_UID,
                _Scene.MULTI_INDEX_ATTR_SERVICE_ADDR)

    def __init__(self, scene_id, scene_uid, service_addr):
        self.scene_id = scene_id
        self.scene_uid = scene_uid
        self.service_addr = service_addr
        self.player_list = []


class SceneMgr(object):

    def __init__(self, service):
        self._service = service
        # self._all_scene = {}    # {scene_id:{scene_uid: scene}}
        # self._scene_by_uid = {}
        self._players = {}  # {role_id: scene_uid}
        self._scene_mi_container = MultiIndexContainer(_Scene)

    def get_scene_by_uid(self, scene_uid):
        return self._scene_mi_container.get_one_elem(_Scene.MULTI_INDEX_ATTR_SCENE_UID, scene_uid)

    def get_scenes_by_id(self, scene_id):
        return self._scene_mi_container.get_elems(_Scene.MULTI_INDEX_ATTR_SCENE_ID, scene_id)

    def reg_scene(self, scene_id, scene_uid, service_addr):
        scene = self.get_scene_by_uid(scene_uid)
        if scene is not None:
            logger.log_error("scene uid has exist, scene_id:{}, scene_uid:{}", scene_id, scene_uid)
            self._scene_mi_container.remove_elem(scene)
        scene = _Scene(scene_id, scene_uid, service_addr)
        self._scene_mi_container.add_elem(scene)
        logger.log_info("reg scene, scene_id:{}, scene_uid:{}", scene_id, scene_uid)

    def unreg_scene(self, scene_uid):
        scene = self.get_scene_by_uid(scene_uid)
        if scene is None:
            logger.log_error("unreg scene not found, scene_uid:{}", scene_uid)
            return
        self._scene_mi_container.remove_elem(scene)
        logger.log_info("unreg scene, scene_id:{}, scene_uid:{}", scene.scene_id, scene_uid)

    def get_min_player_scene(self, scene_id):
        scenes = self.get_scenes_by_id(scene_id)
        if scenes is None:
            return None
        min_count = 0x0FFFFFFF
        tgt_scene = None
        for scene in scenes:
            scene_player_count = len(scene.player_list)
            if min_count > scene_player_count:
                min_count = scene_player_count
                tgt_scene = scene
        return tgt_scene

    def reg_player_to_scene(self, role_id, scene_uid):
        scene = self.get_scene_by_uid(scene_uid)
        if scene is None:
            logger.log_error("reg player to scene not found, role_id:{}, scene_uid:{}", role_id, scene_uid)
            return
        if role_id not in scene.player_list:
            scene.player_list.append(role_id)
        self._players[role_id] = scene_uid

    def unreg_player_to_scene(self, role_id, scene_uid):
        scene = self.get_scene_by_uid(scene_uid)
        if scene is None:
            logger.log_error("unreg player to scene not found, role_id:{}, scene_uid:{}", role_id, scene_uid)
            return
        if role_id in scene.player_list:
            scene.player_list.remove(role_id)
        self._players.pop(role_id, None)


