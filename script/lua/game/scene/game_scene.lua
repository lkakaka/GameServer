
require("base.object")
require("util.logger")
require("game.scene.game_player")

clsGameScene = clsObject:Inherit("clsGameScene")

function clsGameScene:__init__(sceneId)
    self.sceneId = sceneId
    print("clsGameScene:__init__")
    self._engineObj = SceneMgr.createScene(sceneId, self)
    self.sceneUid = self._engineObj:getSceneUid()
    local navMeshName = "../res/" .. "all_tiles_navmesh.bin"
    -- print(type(self._engineObj))
    self._engineObj:loadNavMesh(navMeshName)
    -- print("game scene c++ obj", self._engineObj)
    logger.logInfo("create scene, sceneId=%d, sceneUid=%d", self.sceneId, self.sceneUid)
end

function clsGameScene:createPlayer(connId, roleId, name, x, y)
    local engineObj = self._engineObj:createPlayer(connId, roleId, name, x, y)
    local player = clsPlayer:New(self, engineObj, roleId, name)
    print(type(player))
end

