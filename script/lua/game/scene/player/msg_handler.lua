require("base.object")
require("util.logger")
require("util.str_util")

clsMsgHandler = clsObject:Inherit("clsMsgHandler")

function clsMsgHandler:__init__(player)
    self.player = player
end

function clsMsgHandler:on_recv_client_msg(msg_id, msg)
    -- print("clsMsgHandler:on_recv_client_msg", type(msg_id), clsMsgHandler._c_cmd[msg_id])
    local func = clsMsgHandler._c_cmd[msg_id]
    if func == nil then
        logger.logError("player on_recv_client_msg error, not found cmd func, msgId:%d", msg_id)
        return
    end
    func(self, msg_id, msg)
end

function clsMsgHandler:_on_recv_disconnect(msg_id, msg)
    logger.logInfo("player disconnect, role_id:%d", self.player.role_id)
    self.player.game_scene:tick_player(self.player.role_id, "client req disconnect")
end

function clsMsgHandler:_on_recv_client_disconnect(msg_id, msg)
    logger.logError("player disconnect, role_id:%d,reason:%s", self.player.role_id, msg.reason)
    self.player.game_scene:tick_player(self.player.role_id, msg.reason)
end

function clsMsgHandler:_on_recv_gm_cmd(msg_id, msg)
    local param = {}
    param.player = self.player
    param.args = msg.args
    local err_code, result = self.player.game_scene.service.gm_handler:handle_gm_cmd(msg.cmd, param)
    logger.logInfo("exe gm cmd:%s, args:%s, result:\n%s", msg.cmd, msg.args, result)
    if err_code == ErrorCode.OK then
        self.player:sendGMCmdRsp(msg.cmd, result)
    end
end

function clsMsgHandler:_on_recv_test_req(msg_id, msg)
    local game_scene = self.player.game_scene
    print("player recv test req, role_id:%d, msg:%s", self.player.role_id, msg)
    local rsp_msg = {}
    rsp_msg.id = 10
    rsp_msg.msg = string.format("[lua]welcome to game world, %s, server_id:%d, scene_id:%d, scene_uid:%d",
                                self.player.name, SERVER_GROUP_ID, game_scene.scene_id, game_scene.scene_uid)
    self.player:send_msg_to_client_kcp(MSG_ID_TEST_REQ, rsp_msg)
    self.player:send_msg_to_client(MSG_ID_TEST_REQ, rsp_msg)
    self.player:send_msg_to_sight(MSG_ID_TEST_REQ, rsp_msg)
    local db_addr = make_db_addr(self.player.server_id)
    self.player:send_msg_to_service(db_addr, MSG_ID_TEST_REQ, msg)

    -- local function on_load_cb(err_code, tbl)
    --     print("on_load_cb----------", err_code, tbl)
    -- end

    -- local future = game_scene.service.db_proxy:load(-1, "player", {role_id=1})
    -- future:regCallback(on_load_cb)

    -- local tbl_player = {}
    -- tbl_player.role_id = 6
    -- tbl_player.role_name = "rename"
    -- -- # tbl_player.account = "aa"
    -- game_scene.service.db_proxy:update_one(-1, "player", tbl_player)
    local start_pos = {15, 10, -47}
    local end_pos = {43, 10, -1}
    -- # start_pos = {-665610, 0, -689073}
    -- # end_pos = {-641419, 0, -709361}
    local path = game_scene._engineObj.findPath(game_scene._engineObj, start_pos, end_pos)
    logger.logInfo("find path %s->%s, %s", StrUtil.tableToStr(start_pos), StrUtil.tableToStr(end_pos), StrUtil.tableToStr(path))
end

clsMsgHandler._c_cmd = {
    [MSG_ID_CLIENT_DISCONNECT] = clsMsgHandler._on_recv_client_disconnect,
    [MSG_ID_DISCONNECT_REQ] = clsMsgHandler._on_recv_disconnect,
    [MSG_ID_GM_CMD] = clsMsgHandler._on_recv_gm_cmd,
    [MSG_ID_TEST_REQ] = clsMsgHandler._on_recv_test_req,
}