from util import logger


class _Scene(object):
    def __init__(self, scene_id, scene_uid):
        self.scene_id = scene_id
        self.scene_uid = scene_uid


class SceneMgr(object):

    def __init__(self, service):
        self._service = service
        self._all_scene = {}

    def reg_scene(self, scene_id, scene_uid):
        if scene_id not in self._all_scene:
            self._all_scene[scene_id] = {}
        scenes = self._all_scene[scene_id]
        if scene_uid in scenes:
            logger.logError("$scene uid has exist, scene_id:{}, scene_uid:{}", scene_id, scene_uid)
        self._all_scene[scene_id][scene_uid] = _Scene(scene_id, scene_uid)
        logger.logInfo("$reg scene, scene_id:{}, scene_uid:{}", scene_id, scene_uid)

    def unreg_scene(self, scene_id, scene_uid):
        scenes = self._all_scene.get(scene_id, None)
        if scenes is None:
            logger.logError("$unreg scene not found, scene_id:{}, scene_uid:{}", scene_id, scene_uid)
            return
        scenes.pop(scene_uid, None)
        logger.logInfo("$unreg scene, scene_id:{}, scene_uid:{}", scene_id, scene_uid)
