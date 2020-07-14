from game.util import logger


class _Scene(object):
    def __init__(self, scene_id, scene_uid, service_name):
        self.scene_id = scene_id
        self.scene_uid = scene_uid
        self.service_name = service_name
        self.player_list = []


class SceneMgr(object):

    def __init__(self, service):
        self._service = service
        self._all_scene = {}    # {scene_id:{scene_uid: scene}}
        self._scene_by_uid = {}
        self._players = {}  # {role_id: scene_uid}

    def reg_scene(self, scene_id, scene_uid, service_name):
        if scene_id not in self._all_scene:
            self._all_scene[scene_id] = {}
        scenes = self._all_scene[scene_id]
        if scene_uid in scenes:
            logger.log_error("scene uid has exist, scene_id:{}, scene_uid:{}", scene_id, scene_uid)
        scene = _Scene(scene_id, scene_uid, service_name)
        self._all_scene[scene_id][scene_uid] = scene
        self._scene_by_uid[scene_uid] = scene
        logger.log_info("reg scene, scene_id:{}, scene_uid:{}", scene_id, scene_uid)

    def unreg_scene(self, scene_id, scene_uid):
        scenes = self._all_scene.get(scene_id, None)
        if scenes is None:
            logger.log_error("unreg scene not found, scene_id:{}, scene_uid:{}", scene_id, scene_uid)
            return
        scenes.pop(scene_uid, None)
        self._scene_by_uid.pop(scene_uid, None)
        logger.log_info("unreg scene, scene_id:{}, scene_uid:{}", scene_id, scene_uid)

    def get_min_player_scene(self, scene_id):
        scenes = self._all_scene.get(scene_id, None)
        if scenes is None:
            return None
        min_count = 0x0FFFFFFF
        tgt_scene = None
        for scene_uid, scene in scenes.items():
            scene_player_count = len(scene.player_list)
            if min_count > scene_player_count:
                min_count = scene_player_count
                tgt_scene = scene
        return tgt_scene

    def reg_player_to_scene(self, role_id, scene_uid):
        scene = self._scene_by_uid.get(scene_uid, None)
        if scene is None:
            logger.log_error("reg player to scene not found, role_id:{}, scene_uid:{}", role_id, scene_uid)
            return
        if role_id not in scene.player_list:
            scene.player_list.append(role_id)
        self._players[role_id] = scene_uid

    def unreg_player_to_scene(self, role_id, scene_uid):
        scene = self._scene_by_uid.get(scene_uid, None)
        if scene is None:
            logger.log_error("unreg player to scene not found, role_id:{}, scene_uid:{}", role_id, scene_uid)
            return
        if role_id in scene.player_list:
            scene.player_list.remove(role_id)
        self._players.pop(role_id, None)


