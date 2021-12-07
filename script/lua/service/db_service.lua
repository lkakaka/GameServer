require("service.service_base")
require("util.logger")
require("game.db.db_handler")
require("util.const")
require("util.table_util")
require("base.id_mgr")

clsDBService = clsServiceBase:Inherit("clsDBService")

function clsDBService:__init__()
    Super(clsDBService).__init__(self)
    logger.logInfo("clsDBService:__init__")
    local db_name = Config:getConfigStr("db_name")
    local redis_ip = Config:getConfigStr("redis_ip")
    local redis_port = Config:getConfigInt("redis_port")
    self.db_hander = clsDBHandler:New(db_name, redis_ip, redis_port)
    self:initRpcHandler()
    self:reg_service_msg_handler(MSG_ID_TEST_REQ, self._on_recv_test_req)
end

function clsDBService:initRpcHandler()
    self:reg_rpc_handler("RpcLoad", self.rpc_load)
    self:reg_rpc_handler("RpcLoadMulti", self.rpc_load_multi)
    self:reg_rpc_handler("RpcInsert", self.rpc_insert)
    self:reg_rpc_handler("RpcUpdate", self.rpc_update)
    self:reg_rpc_handler("RpcDelete", self.rpc_delete)

    self:reg_rpc_handler("CreateRole", self.rpc_create_role)
end

function clsDBService:rpc_load(sender, tbl)
    local result = self.db_hander:load(tbl)
    
    return ErrorCode.OK, result
end

function clsDBService:rpc_load_multi(sender, tbls)
    print(StrUtil.tableToStr(tbls))
    local results = {}
    for _,tbl in ipairs(tbls) do
        local result = self.db_hander:load(tbl)
        results[tbl.table_name] = result
    end
    return ErrorCode.OK, results
end

function clsDBService:rpc_insert(sender, tbl)
    local result = self.db_hander:insert_table(tbl)
    return ErrorCode.OK, result
end

function clsDBService:rpc_update(sender, tbl)
    local result = self.db_hander:update_table(tbl)
    return ErrorCode.OK, result
end

function clsDBService:rpc_delete(sender, tbl)
    local result = self.db_hander:delete_table(tbl)
    return ErrorCode.OK, result
end

function clsDBService:rpc_create_role(sender, args)
    local conn_id = args.conn_id
    local account = args.account
    local role_name = args.role_name
    -- print("_on_rpc_create_role----", conn_id, account, role_name)
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

    local role_id = IDMgr.alloc_role_id(1)

    self.db_hander:execute_sql(string.format("insert into player(role_id, role_name, account) values(%d, '%s', '%s')", role_id, role_name, account))
    db_res = self.db_hander:execute_sql(string.format("select * from player where role_name='%s'", role_name))

    rsp_msg.role_info.role_id = db_res[1].role_id
    rsp_msg.role_info.role_name = role_name
    rsp_msg.err_code = ErrorCode.OK
    self:sendMsgToClient(conn_id, MSG_ID_CREATE_ROLE_RSP, rsp_msg)
    logger.logInfo("create role, role_id:%d, role_name:%s, account:%s", role_id, role_name, account)
end


function clsDBService:_on_recv_test_req(sender, msg_id, msg)
    print("-----recv test msg", msg.id, msg.msg)
    -- local tbl_player = { table_name = "player", row = {account = "test"} }
    -- local db_res = self.db_hander:load(tbl_player)
    -- print(db_res)
    -- for _,row in ipairs(db_res.rows) do
    --     print(StrUtil.tableToStr(row))
    -- end
    self.db_hander:redis_benchmark()
end