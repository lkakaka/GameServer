require("base.object")
require("util.logger")

clsGamePlayer = clsObject:Inherit("clsGamePlayer")

function clsGamePlayer:__init__(gameScene, engineObj, roleId, name)
    self.gameScene = gameScene
    self.engineObj = engineObj
    self.roleId = roleId
    self.name = name
    self.connId = engineObj:getConnId()
end