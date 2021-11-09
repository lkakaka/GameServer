require("base.object")
require("service.service_base")
require("base.service_type")
require("proto.pb_message")
require("util.logger")
require("util.const")
require("util.crypt")
require("os")
require("math")
require("base.http_server")
require("util.str_util")

clsLoginService = clsServiceBase:Inherit("clsLoginService")

function clsLoginService:__init__()
    Super(clsLoginService).__init__(self)
    logger.logInfo("clsLoginService:__init__")
    self._account_dict = {}
    self._conn_dict = {}
    self:initClientMsgHandler()
    self.http_server = clsHttpServer:New(8088)
    -- HttpClient.sendHttpReq("www.baidu.com", function(resp)
    --     logger.logInfo("http resp, %d, %s", resp.status, resp.content)
    -- end)
end

function clsLoginService:initClientMsgHandler()
    self:regClientMsgHandler(MSG_ID_LOGIN_REQ, self.loginReq)
    self:regClientMsgHandler(MSG_ID_CREATE_ROLE_REQ, self.createRoleReq)
    self:regClientMsgHandler(MSG_ID_ENTER_GAME, self.enterGameReq)
end

function clsLoginService:loginReq(conn_id, msg_id, msg)
    local rsp_msg = { err_code = ErrorCode.OK }
    -- if msg.account in self._account_dict:
    --     rsp_msg.err_code = ErrorCode.ACCOUNT_IS_LOGINING
    --     self:sendMsgToClient(conn_id, MessageObj.MSG_ID_LOGIN_RSP, rsp_msg)
    --     return
    -- todo:验证账号

    self._conn_dict[conn_id] = msg.account
    logger.logInfo("recv login req conn_id=%d", conn_id)
    self:sendMsgToClient(conn_id, MSG_ID_LOGIN_RSP, rsp_msg)

    self._account_dict[msg.account] = conn_id
    self:_load_role_list(conn_id, msg.account)
end

function clsLoginService:_load_role_list(conn_id, account)
    local function on_load_role_list(err_code, tbl)
        print("on_load_role_list---", err_code, StrUtil.tableToStr(tbl))
        self:_on_load_role_list(account, err_code, tbl)
    end

    local future = self.db_proxy:load("player", {account=account})
    future:regCallback(on_load_role_list)
end

function clsLoginService:_on_load_role_list(account, err_code, tbl)
    local conn_id = self._account_dict[account]
    if conn_id == nil then
        logger.logError("not found account's conn id, account:%s", account)
        return
    end
    local rsp_msg = {role_list = {}}
    rsp_msg.account = account
    rsp_msg.err_code = err_code
    for _,row in ipairs(tbl.rows) do
        local role_info = {}
        role_info.role_id = row.role_id
        role_info.role_name = row.role_name
        table.insert(rsp_msg.role_list, role_info)
    end
    self:sendMsgToClient(conn_id, MSG_ID_LOAD_ROLE_LIST_RSP, rsp_msg)
end

function clsLoginService:createRoleReq(conn_id, msg_id, msg)
    local rsp_msg = {}
    local account = self._conn_dict[conn_id]
    if account == nil or account ~= msg.account then
        rsp_msg.err_code = ErrorCode.CONN_INVALID
        self:sendMsgToClient(conn_id, MSG_ID_CREATE_ROLE_RSP, rsp_msg)
        logger.logError("create role error, account:%s, msg.account:%s", account, msg.account)
        return
    end

    self:callRpc(LOCAL_SERVICE_DB, "CreateRole", -1, {conn_id=conn_id, account=msg.account, role_name=msg.role_name})
end

function clsLoginService:enterGameReq(conn_id, msg_id, msg)
    local account = self._conn_dict[conn_id]
    if account == nil then
        logger.logError("enter game error, conn_id(%d) invalid", conn_id)
        self:_send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
        return
    end

    local function on_load_role(err_code, tbl)
        self:_on_load_role(conn_id, err_code, tbl)
    end

    local future = self.db_proxy:load("player", {role_id=msg.role_id})
    future:regCallback(on_load_role)
    logger.logInfo("enter game req, conn_id:%d, account:%s", conn_id, account)
end


function clsLoginService:_on_load_role(conn_id, err_code, tbl)
    local account = self._conn_dict[conn_id]
    if account == nil then
        logger.logError("load role rsp, conn_id(%d) invalid", conn_id)
        self:_send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
        return
    end

    if tbl == nil or TableUtil.isEmpty(tbl.rows) then
        logger.logError("load role rsp, role data not exist, account:%s", account)
        self:_send_enter_game_rsp(conn_id, ErrorCode.CONN_INVALID)
        return
    end

    local row = tbl.rows[1]

    local function _on_query_login_scene(err_code, result)
        if err_code ~= ErrorCode.OK then
            self:_send_enter_game_rsp(conn_id, err_code)
            return
        end
        self:_send_enter_game_rsp(conn_id, ErrorCode.OK, row)
        self._conn_dict[conn_id] = nil
        self._account_dict[account] = nil
        self:_send_kcp_start_info(conn_id)
    end

    local future = self:callRpc(LOCAL_SERVICE_SCENE_CTRL, "Player_EnterGame", 30, {conn_id=conn_id, role_id=row.role_id})
    future:regCallback(_on_query_login_scene)
    logger.logInfo("send enter scene req to scene ctrl, conn_id:%d, account:%s", conn_id, account)
end


function clsLoginService:_send_kcp_start_info(conn_id)
    local rand_num = math.random(1, 10000)
    local str_token = TokenPrefix.KCP .. tostring(conn_id) .. tostring(os.time()) .. tostring(rand_num)
    local msg = {}
    msg.kcp_id = conn_id;
    msg.token = Crypt.md5(str_token)
    self:sendMsgToClient(conn_id, MSG_ID_START_KCP, msg)
end

function clsLoginService:_send_enter_game_rsp(conn_id, err_code, tbl_player)
    local msg = {}
    msg.err_code = err_code
    if tbl_player ~= nil then
        msg.role_info = {}
        msg.role_info.role_id = tbl_player.role_id
        msg.role_info.role_name = tbl_player.role_name
    end
    self:sendMsgToClient(conn_id, MSG_ID_ENTER_GAME_RSP, msg)
end
