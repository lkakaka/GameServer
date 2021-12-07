require("base.object")
require("util.logger")
require("util.multi_index_elem")

PlayerState = {
    UNKNOW = 0,      -- 未知状态
    LOGINING = 1,    -- 登录中
    IN_SCENE = 2,    -- 在场景中
    SWITCHING = 3,   -- 切换场景中
    REMOTE_SWITCHING = 4,   -- 切换到其他服场景中
}

clsPlayerInfo = clsMultiIndexElem:Inherit("clsPlayerInfo")

clsPlayerInfo.index_roleId = "role_id"
clsPlayerInfo.index_connId = "conn_id"

function clsPlayerInfo:__init__(role_id, conn_id)
    Super(clsPlayerInfo).__init__(self, {clsPlayerInfo.index_roleId, clsPlayerInfo.index_connId})
    self.role_id = role_id
    self.conn_id = conn_id
    self.state = PlayerState.UNKNOW
    self.scene_uid = 0
    self.scene_id = 0
end

function clsPlayerInfo:change_conn_id(conn_id)
    if self.conn_id == conn_id then return end
    Super(clsPlayerInfo).change_index_attr_value(self, clsPlayerInfo.index_connId, conn_id)
end
