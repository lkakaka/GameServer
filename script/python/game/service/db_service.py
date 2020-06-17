from util import logger
from proto.pb_message import Message
from game.service.service_base import ServiceBase
import util.cmd_util
from util.const import ErrorCode
from game.db.db_handler import DBHandler
from game.db.db_builder import DbInfo
import util.db_util
import game.db.tbl


CUR_DB_VERSION = 1


class DBService(ServiceBase):

    _s_cmd = util.cmd_util.CmdDispatch("db_service")
    _rpc_proc = util.cmd_util.CmdDispatch("rpc_db_service")

    def __init__(self):
        ServiceBase.__init__(self, DBService._s_cmd, None, DBService._rpc_proc)
        self._db_handler = None
        self._db_info = None

    def on_service_start(self):
        logger.log_info("DBService start!!!")
        # DBHandler.test_db()
        self._db_handler = DBHandler("save")
        self._db_info = DbInfo(self._db_handler)
        # from game.db.tbl.tbl_player import TblPlayer
        # from game.db.tbl.tbl_item import TblItem
        # from game.db.tbl.tbl_test import TblTest
        # tbls = (TblTest, TblPlayer, TblItem)
        # logger.log_info("$start init table, {}", tbls)
        # if not self._db_handler.init_table(tbls):
        #     raise RuntimeError("init tables error")
        # logger.log_info("$finish init table, {}", tbls)

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

        # self._db_handler.flush_add_clean_redis()
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

    @_s_cmd.reg_cmd(Message.MSG_ID_LOAD_ROLE_LIST_REQ)
    def _on_recv_load_role_list_req(self, sender, msg_id, msg):
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_LOAD_ROLE_LIST_RSP)
        rsp_msg.account = msg.account
        db_res = self._db_handler.execute_sql("select * from player where account='{}'".format(msg.account))
        if db_res is None:
            rsp_msg.err_code = util.const.ErrorCode.DB_ERROR
            logger.log_error("loader player list error!!!, account:{}", msg.account)
        else:
            rsp_msg.err_code = util.const.ErrorCode.OK
            for res in db_res:
                role_info = rsp_msg.role_list.add()
                role_info.role_id = res.role_id
                role_info.role_name = res.role_name
                # rsp_msg.role_list.append(role_info)
        self.send_msg_to_service(sender, rsp_msg)

    @_s_cmd.reg_cmd(Message.MSG_ID_LOAD_ROLE_REQ)
    def _on_recv_load_role_req(self, sender, msg_id, msg):
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_LOAD_ROLE_RSP)
        rsp_msg.conn_id = msg.conn_id
        db_res = self._db_handler.execute_sql("select * from player where role_id={}".format(1))
        rsp_msg.role_info.role_id = db_res[0].role_id
        rsp_msg.role_info.role_name = db_res[0].role_name
        self.send_msg_to_service(sender, rsp_msg)

    @_rpc_proc.reg_cmd("CreateRole")
    def _on_rpc_create_role(self, sender, conn_id, account, role_name):
        print("_on_rpc_create_role----", conn_id, account, role_name)
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_CREATE_ROLE_RSP)
        db_res = self._db_handler.execute_sql("select * from player where account='{}'".format(account))
        if len(db_res) >= util.const.GlobalVar.MAX_ROLE_NUM:
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
        self._db_handler.redis_benchmark()

