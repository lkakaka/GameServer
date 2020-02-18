
class DbInfo:
    DB_INFO_KEY_VERSION = 1

    def __init__(self, db_handler):
        self._db_handler = db_handler
        self.init_db_info()

    def init_db_info(self):
        sql = "create table if not exists _db_info(key_id int not null primary key, val_int int, val_str text)"
        self._db_handler.execute_sql(sql)

    def get_int_val(self, key_id, def_val=None):
        sql = "select * from _db_info where key_id = {}".format(key_id)
        res = self._db_handler.execute_sql(sql)
        if res is None:
            raise RuntimeError("get db info failed, key_id:" + key_id)
            return None

        if len(res) == 0:
            return def_val

        return res[0].val_int

    def get_str_val(self, key_id, def_val=None):
        sql = "select * from _db_info where key_id = {}".format(key_id)
        res = self._db_handler.execute_sql(sql)
        if res is None:
            raise RuntimeError("get db info failed, key_id:" + key_id)
            return None

        if len(res) == 0:
            return def_val

        return res[0].val_str

    def set_int_val(self, key_id, val):
        sql = "replace into _db_info(key_id, val_int) values({}, {})".format(key_id, val)
        res = self._db_handler.execute_sql(sql)
        if res is None or len(res) == 0:
            return False

        return True

    def set_str_val(self, key_id, val):
        sql = "replace into _db_info(key_id, val_str) values({}, '{}')".format(key_id, val)
        res = self._db_handler.execute_sql(sql)
        if res is None or len(res) == 0:
            return False

        return True

