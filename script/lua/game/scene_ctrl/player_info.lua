require("base.object")
require("util.logger")

PlayerState = {
    UNKNOW = 0,      -- 未知状态
    LOGINING = 1,    -- 登录中
    IN_SCENE = 2,    -- 在场景中
    SWITCHING = 3,   -- 切换场景中
}

clsPlayerInfo = clsObject:Inherit("clsPlayerInfo")

clsPlayerInfo.index_roleId = "role_id"
clsPlayerInfo.index_connId = "conn_id"

function clsPlayerInfo:__init__(role_id, conn_id)
    self.role_id = role_id
    self.conn_id = conn_id
    self.state = PlayerState.UNKNOW
    self.scene_uid = 0
    self.scene_id = 0
end
