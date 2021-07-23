require("service.service_base")
require("util.logger")
require("game.db.db_handler")
require("util.const")
require("util.table_util")

clsDBService = clsServiceBase:Inherit("clsDBService")

function clsDBService:__init__()
    Super(clsDBService).__init__(self)
    logger.logInfo("clsDBService:__init__")
    self.db_hander = clsDBHandler:New("save", "127.0.0.1", 6379)
    self:initRpcHandler()
    self:regServiceMsgHandler(MSG_ID_TEST_REQ, self._on_recv_test_req)
end

function clsDBService:initRpcHandler()
    self:regRpcHandler("RpcLoad", self.rpcLoad)
    self:regRpcHandler("RpcLoadMulti", self.rpcLoadMulti)
    self:regRpcHandler("RpcInsert", self.rpcInsert)
    self:regRpcHandler("RpcUpdate", self.rpcUpdate)
    self:regRpcHandler("RpcDelete", self.rpcDelete)

    self:regRpcHandler("CreateRole", self.rpcCreateRole)
end

function clsDBService:rpcLoad(sender, tbl)
    local result = self.db_hander:load(tbl)
    return ErrorCode.OK, result
end

function clsDBService:rpcLoadMulti(sender, tbls)
    print(StrUtil.tableToStr(tbls))
    local results = {}
    for _,tbl in ipairs(tbls) do
        local result = self.db_hander:load(tbl)
        results[tbl.table_name] = result
    end
    return ErrorCode.OK, results
end

function clsDBService:rpcInsert(sender, tbl)
    local result = self.db_hander:insert_table(tbl)
    return ErrorCode.OK, result
end

function clsDBService:rpcUpdate(sender, tbl)
    local result = self.db_hander:update_table(tbl)
    return ErrorCode.OK, result
end

function clsDBService:rpcDelete(sender, tbl)
    local result = self.db_hander:delete_table(tbl)
    return ErrorCode.OK, result
end

function clsDBService:rpcCreateRole(sender, args)
    local conn_id = args.conn_id
    local account = args.account
    local role_name = args.role_name
    print("_on_rpc_create_role----", conn_id, account, role_name)
    local rsp_msg = {role_info = {}}
    local db_res = self.db_hander:execute_sql(string.format("select * from player where account='%s'", account))
    if TableUtil.size(db_res) >= GlobalVar.MAX_ROLE_NUM then
        rsp_msg.err_code = ErrorCode.ROLE_COUNT_LIMIT
        self:sendMsgToClient(conn_id, MSG_ID_CREATE_ROLE_RSP, rsp_msg)
        return
    end
    db_res = self.db_hander:execute_sql(string.format("select count(*) as name_count from player where role_name='%s'", role_name))
    if not TableUtil.isEmpty(db_res) and db_res[1].name_count > 0 then
        rsp_msg.err_code = ErrorCode.ROLE_NAME_EXIST
        self:sendMsgToClient(conn_id, MSG_ID_CREATE_ROLE_RSP, rsp_msg)
        return
    end

    self.db_hander:execute_sql(string.format("insert into player(role_name, account) values('%s', '%s')", role_name, account))
    db_res = self.db_hander:execute_sql(string.format("select * from player where role_name='%s'", role_name))

    rsp_msg.role_info.role_id = db_res[1].role_id
    rsp_msg.role_info.role_name = role_name
    rsp_msg.err_code = ErrorCode.OK
    self:sendMsgToClient(conn_id, MSG_ID_CREATE_ROLE_RSP, rsp_msg)
end


function clsDBService:_on_recv_test_req(sender, msg_id, msg)
    -- local tbl_player = { table_name = "player", row = {account = "test"} }
    -- local db_res = self.db_hander:load(tbl_player)
    -- print(db_res)
    -- for _,row in ipairs(db_res.rows) do
    --     print(StrUtil.tableToStr(row))
    -- end
    self.db_hander:redis_benchmark()
end