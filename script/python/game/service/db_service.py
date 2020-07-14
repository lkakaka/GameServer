
import Config
from game.util import logger
from proto.pb_message import Message
from game.service.service_base import ServiceBase
from game.util.const import ErrorCode
from game.db.db_handler import DBHandler
from game.db.db_builder import DbInfo
import game.util.cmd_util
import game.util.db_util
import game.db.tbl


CUR_DB_VERSION = 1


class DBService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("db_service")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_db_service")

    def __init__(self):
        ServiceBase.__init__(self, DBService._s_cmd, None, DBService._rpc_proc)
        self._db_handler = None
        self._db_info = None

    def _init_id_mgr(self):
        pass

    def on_service_start(self):
        ServiceBase.on_service_start(self)
        logger.log_info("DBService start!!!")
        # DBHandler.test_db()

        db_name = Config.getConfigStr("db_name")
        if not db_name:
            raise RuntimeError("not config db_name")

        redis_ip = Config.getConfigStr("redis_ip")
        redis_port = Config.getConfigInt("redis_port")
        if not redis_ip or not redis_port:
            raise RuntimeError("not config redis_ip or redis_port")

        logger.log_info("db_name:{}, redis_ip:{}, redis_port:{}".format(db_name, redis_ip, redis_port))

        self._db_handler = DBHandler(db_name, redis_ip, redis_port)
        self._db_info = DbInfo(self._db_handler)
        self._db_handler.init_tables(game.db.tbl)

        # db_ver = self._db_info.get_int_val(DbInfo.DB_INFO_KEY_VERSION, -1)
        # if db_ver >= CUR_DB_VERSION:
        #     return
        #
        # if db_ver == -1:
        #     if not self.init_db():
        #         logger.logError("$init db error")
        #         raise RuntimeError("init db error")
        #         return
        # else:
        #     if not self.upgrade_db(db_ver):
        #         logger.logError("$upgrade db error, db_ver:{}".format(db_ver))
        #         raise RuntimeError("upgrade db error, db_ver:{}".format(db_ver))
        #         return
        #
        # if not self._db_info.set_int_val(DbInfo.DB_INFO_KEY_VERSION, CUR_DB_VERSION):
        #     logger.logError("$set db version failed, old_version:{}, cur_version:{}".format(db_ver, CUR_DB_VERSION))
        #     raise RuntimeError("set db version failed, old_version:{}, cur_version:{}".format(db_ver, CUR_DB_VERSION))

        self._db_handler.test_db_and_redis()

    def init_db(self):
        sql = "create table if not exists player(" \
              "role_id int not null primary key auto_increment, " \
              "name varchar(50) not null, " \
              "account varchar(50) not null" \
              ")"
        res = self._db_handler.execute_sql(sql)
        if res is None:
            logger.log_error("create table player error, sql:{}", sql)
            return False
        sql = "create unique index index_account on player(account)"
        res = self._db_handler.execute_sql(sql)
        if res is None:
            logger.log_error("create index index_account error, sql:{}", sql)
            return False

        return True

    def upgrade_db(self, db_ver):
        return True

    @_rpc_proc.reg_cmd("RpcLoadDB")
    def _on_recv_rpc_load_db(self, sender, tb_name, **kwargs):
        tbl_sql = game.util.db_util.create_tbl_obj(tb_name)
        for k, v in kwargs.items():
            tbl_sql[k] = v
        result_list = []
        tbl_list = self._db_handler.load_tb_data(tbl_sql)
        for tbl in tbl_list:
            result_list.append(tbl.to_dict())
        return ErrorCode.OK, tuple(result_list)

    @_rpc_proc.reg_cmd("RpcLoadDBMulti")
    def _on_recv_rpc_load_db_multi(self, sender, tbl_list):
        result_list = []
        for tbl in tbl_list:
            tbl_sql = game.util.db_util.create_tbl_obj(tbl["__tb_name"])
            for k, v in tbl.items():
                if k == "__tb_name":
                    continue
                tbl_sql[k] = v
            res_list = self._db_handler.load_tb_data(tbl_sql)
            for tbl_res in res_list:
                result_list.append(tbl_res.to_dict())
        return ErrorCode.OK, tuple(result_list)

    @_rpc_proc.reg_cmd("RpcInsertDB")
    def _on_recv_rpc_insert_db(self, sender, tbl_list):
        print("_on_recv_rpc_insert_db", tbl_list)
        tbls = []
        for data in tbl_list:
            tbl_sql = game.util.db_util.create_tbl_obj(data["__tb_name"])
            tbl_sql.assign(data)
            tbl_sql.dump_cols()
            tbls.append(tbl_sql)
        if self._db_handler.insert_table(tbls):
            return ErrorCode.OK
        game.util.logger.log_error("insert db error")
        return ErrorCode.DB_ERROR

    @_rpc_proc.reg_cmd("RpcUpdateDB")
    def _on_recv_rpc_update_db(self, sender, tbl_list):
        tbls = []
        for data in tbl_list:
            tbl_sql = game.util.db_util.create_tbl_obj(data["__tb_name"])
            tbl_sql.assign(data)
            tbls.append(tbl_sql)
        if self._db_handler.update_table(tbls):
            return ErrorCode.OK
        return ErrorCode.DB_ERROR

    @_rpc_proc.reg_cmd("RpcDeleteDB")
    def _on_recv_rpc_delete_db(self, sender, tbl_list):
        tbls = []
        for data in tbl_list:
            tbl_sql = game.util.db_util.create_tbl_obj(data["__tb_name"])
            tbl_sql.assign(data)
            tbls.append(tbl_sql)
        if self._db_handler.delete_table(tbls):
            return ErrorCode.OK
        return ErrorCode.DB_ERROR

    @_rpc_proc.reg_cmd("CreateRole")
    def _on_rpc_create_role(self, sender, conn_id, account, role_name):
        print("_on_rpc_create_role----", conn_id, account, role_name)
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_CREATE_ROLE_RSP)
        db_res = self._db_handler.execute_sql("select * from player where account='{}'".format(account))
        if len(db_res) >= game.util.const.GlobalVar.MAX_ROLE_NUM:
            rsp_msg.err_code = ErrorCode.ROLE_COUNT_LIMIT
            self.send_msg_to_client(conn_id, rsp_msg)
            return
        db_res = self._db_handler.execute_sql("select count(*) as name_count from player where role_name='{}'".format(role_name))
        if len(db_res) > 0 and db_res[0].name_count > 0:
            rsp_msg.err_code = ErrorCode.ROLE_NAME_EXIST
            self.send_msg_to_client(conn_id, rsp_msg)
            return

        self._db_handler.execute_sql("insert into player(role_name, account) values('{}', '{}')".format(role_name, account))
        db_res = self._db_handler.execute_sql("select * from player where role_name='{}'".format(role_name))

        # from game.db.tbl.tbl_player import TblPlayer
        # tbl_player = TblPlayer()
        # tbl_player.account = account
        # db_res = self._db_handler.get_row(tbl_player)

        rsp_msg.role_info.role_id = db_res[0].role_id
        rsp_msg.role_info.role_name = role_name
        rsp_msg.err_code = ErrorCode.OK
        self.send_msg_to_client(conn_id, rsp_msg)

    @_s_cmd.reg_cmd(Message.MSG_ID_TEST_REQ)
    def _on_recv_test_req(self, sender, msg_id, msg):
        # import game.db.tbl.tbl_player
        # tbl_player = game.db.tbl.tbl_player.TblPlayer()
        # tbl_player.account = "test"
        # db_res = self._db_handler.load_tb_data(tbl_player)
        # print(db_res)
        # for res in db_res:
        #     res.dump_cols()
        # self._db_handler.redis_benchmark()
        pass

