import logger
from proto.pb_message import Message
from game.service.service_base import ServiceBase
import game.util.cmd_util
from game.db.db_handler import DBHandler
from game.db.db_builder import DbInfo


CUR_DB_VERSION = 1


class DBService(ServiceBase):

    _s_cmd = game.util.cmd_util.CmdDispatch("db_service")

    def __init__(self):
        ServiceBase.__init__(self, DBService._s_cmd, None)
        self._db_handler = None
        self._db_info = None

    def on_service_start(self):
        logger.logInfo("$DBService start!!!")
        # DBHandler.test_db()
        self._db_handler = DBHandler("save")
        self._db_info = DbInfo(self._db_handler)
        db_ver = self._db_info.get_int_val(DbInfo.DB_INFO_KEY_VERSION, -1)
        if db_ver >= CUR_DB_VERSION:
            return

        if db_ver == -1:
            if not self.init_db():
                logger.logError("$init db error")
                raise RuntimeError("init db error")
                return
        else:
            if not self.upgrade_db(db_ver):
                logger.logError("$upgrade db error, db_ver:{}".format(db_ver))
                raise RuntimeError("upgrade db error, db_ver:{}".format(db_ver))
                return

        if not self._db_info.set_int_val(DbInfo.DB_INFO_KEY_VERSION, CUR_DB_VERSION):
            logger.logError("$set db version failed, old_version:{}, cur_version:{}".format(db_ver, CUR_DB_VERSION))
            raise RuntimeError("set db version failed, old_version:{}, cur_version:{}".format(db_ver, CUR_DB_VERSION))

    def init_db(self):
        sql = "create table if not exists player(" \
              "role_id int not null primary key auto_increment, " \
              "name varchar(50) not null, " \
              "account varchar(50) not null" \
              ")"
        res = self._db_handler.execute_sql(sql)
        if res is None:
            logger.logError("$create table player error, sql:{}", sql)
            return False
        sql = "create unique index index_account on player(account)"
        res = self._db_handler.execute_sql(sql)
        if res is None:
            logger.logError("$create index index_account error, sql:{}", sql)
            return False

        return True

    def upgrade_db(self, db_ver):
        return True

    # # 使用基类方法(windows release版会崩,为何？)
    # def on_recv_service_msg(self, sender, msg_id, msg_data):
    #     logger.logInfo("$DBService on_recv_msg, sender:{}, msg_id:{}", sender, msg_id)
    #     msg = Message.create_msg_by_id(msg_id)
    #     msg.ParseFromString(msg_data)
    #     func = DBService._s_cmd.get_cmd_func(msg_id)
    #     if func is None:
    #         logger.logError("$not cmd func found, msg_id:{}", msg_id)
    #         return
    #     func(self, sender, msg_id, msg)

    @_s_cmd.reg_cmd(Message.MSG_ID_LOGIN_REQ)
    def _on_recv_login_req(self, sender, msg_id, msg):
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_LOGIN_RSP)
        rsp_msg.account = msg.account
        db_res = self._db_handler.execute_sql("select * from player where account='{}'".format(msg.account))
        if len(db_res) == 0:
            self._db_handler.execute_sql("insert into player(name, account) values('{}', '{}')".format("", msg.account))
            db_res = self._db_handler.execute_sql("select * from player where account='{}'".format(msg.account))
        if db_res is None or len(db_res) == 0:
            rsp_msg.err_code = 1
            logger.logError("$create player error!!!")
        else:
            rsp_msg.user_id = db_res[0].role_id
            rsp_msg.conn_id = msg.conn_id
            rsp_msg.err_code = 0
        self.send_msg_to_service(sender, Message.MSG_ID_LOGIN_RSP, rsp_msg)


