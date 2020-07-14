
import PyDb
from game.util import logger
from game.db.db_redis import DBRedis
import game.util.timer


class DBRowResult:
    def __init__(self, col_name_idx, row_data):
        self._col_name_idx = col_name_idx
        self._row_dat = row_data

    # def __iter__(self):
    #     return self._row_dat.__iter__()

    def __getattr__(self, item):
        if item not in self._col_name_idx:
            raise AttributeError
        return self._row_dat[self._col_name_idx[item]]


class DBHandler:

    INDEX_ALL_CACHED = "ALL_CACHED"
    INDEX_NOT_ALL_CACHED_SCORE = "0"
    INDEX_ALL_CACHED_SCORE = "1"
    INDEX_PRI_KEY_SCORE = "10"

    REDIS_FLUSH_TIME = 5 * 6

    def __init__(self, db_name, redis_ip, redis_port):
        self._db_name = db_name
        self._db_inst = PyDb.DbInst(self._db_name)
        self._db_redis = DBRedis(redis_ip, redis_port)
        self._flush_timer_id = game.util.timer.add_timer(DBHandler.REDIS_FLUSH_TIME, lambda: self._on_flush_redis(), loop_cnt=-1)

    @property
    def name(self):
        return self._db_name

    def execute_sql(self, sql):
        result = self._db_inst.executeSql(sql)
        if result is None:
            return None

        col_names = result.pop(0)
        col_name_idx = {}
        for idx in range(len(col_names)):
            col_name_idx[col_names[idx]] = idx

        db_res = []
        for row_result in result:
            db_row_res = DBRowResult(col_name_idx, row_result)
            db_res.append(db_row_res)
        return tuple(db_res)

    def insert_row(self, tbl):
        return self._db_inst.insertRow(tbl)

    def update_row(self, tbl):
        return self._db_inst.updateRow(tbl)

    def replace_rows(self, tbls):
        return self._db_inst.replaceRows(tbls)

    def get_row(self, tbl):
        # tbl.tb_name = tbl.tb_name
        return self._db_inst.getRow(tbl)

    def delete_row(self, tbl):
        return self._db_inst.deleteRow(tbl)

    def init_tables(self, mod):
        import os
        import sys
        tbls = []
        tbl_names = []
        list_dir = os.walk(mod.__path__[0])
        for dirPath, _, files in list_dir:
            for fname in files:
               if not fname.endswith(".py") or not fname.startswith("tbl_"):
                   continue
               fname = fname[0:-3]
               mod_name = mod.__name__ + "." + fname
               __import__(mod_name)
               mod_dict = sys.modules[mod_name].__dict__
               pos = fname.find("_")
               tbl_name = fname[pos+1:]
               tbl_names.append(tbl_name)
               cls_name = "Tbl" + tbl_name.capitalize()
               if cls_name not in mod_dict:
                   raise RuntimeError("class {} not file {}".format(cls_name, fname))
               tbls.append(mod_dict[cls_name])

        logger.log_info("*********init table, {}", tbls)
        if not self._db_inst.initTable(tuple(tbls)):
            raise RuntimeError("init tables error")
        self.flush_add_clean_redis()
        self._init_id_allocator(tbl_names)
        return tbl_names

    def _init_id_allocator(self, tbl_names):
        redis_cmd = "HMSET _id_allocator"
        for tbl_name in tbl_names:
            tbl = game.util.db_util.create_tbl_obj(tbl_name)
            if tbl.primary_col is None:
                continue
            # sql = "select auto_increment from information_schema.TABLES where TABLE_NAME='{}'".format(tbl_name)
            sql = "select max({}) as max_id from {}".format(tbl.primary_col.name, tbl_name)
            res = self.execute_sql(sql)
            max_id = res[0].max_id
            redis_cmd += " {} {}".format(tbl_name, max_id)
        if self._db_redis.exec_redis_cmd(redis_cmd) is None:
            raise RuntimeError("init id allocator error, redis cmd:{}".format(redis_cmd))

    def load_tb_data(self, tbl):
        primary_val = tbl.primary_val
        if primary_val is not None:
            key = tbl.make_redis_pri_key()
            return self._load_data_by_primary_key(tbl, key)
        tb_index = tbl.match_index()
        if tb_index is not None:
            print("match_index----", tb_index.cols)
            return self._load_data_by_index(tbl, tb_index)
        logger.log_warn("load data from db, may be create index on table {}", tbl.tb_name)
        return self._load_from_db_and_cache(tbl)

    def _load_data_by_primary_key(self, tbl, primary_key):
        result = self._load_data_from_redis(tbl.tb_name, primary_key)
        if result is not None:
            return result
        tbl_sql = tbl.clone(col_names=(tbl.primary_col.name,))
        return self._load_from_db_and_cache(tbl_sql)

    def _load_data_from_redis(self, tb_name, primary_key):
        result = self._db_redis.exec_redis_cmd("HGETALL {}", primary_key)
        if result:
            res_tbl = game.util.db_util.create_tbl_obj(tb_name)
            for i in range(0, len(result), 2):
                res_tbl[result[i]] = result[i+1]
            return (res_tbl,)
        return None

    def _load_data_by_index(self, tbl, tb_index):
        result_lst = []
        key = tbl.make_redis_index_key(tb_index)
        result = self._db_redis.exec_redis_cmd("ZRANGE {} 0 -1 WITHSCORES", key)
        print("redis result----", result)
        if result and result[1] == DBHandler.INDEX_ALL_CACHED_SCORE:    # 已在redis中全缓存
            for i in range(2, len(result), 2):
                pri_key = result[i]
                if pri_key == "":
                    continue
                tup = self._load_data_by_primary_key(tbl, pri_key)
                for res in tup:
                    if not tbl.fiter(res):
                        continue
                    result_lst.append(res)
            return result_lst
        tbl_sql = tbl.clone(col_names=tb_index.cols)
        print("tbl sql-===", tbl_sql)
        lst = self._load_from_db_and_cache(tbl_sql)
        self._cache_tbl_index_to_redis(lst, tb_index, True)
        return lst

    def _cache_tbl_index_to_redis(self, tbls, tb_index, is_all_cached):
        # ZADD key score member [[score member] [score member] …]
        # score=0,1：代表是否已经全部缓存在redis中
        # score=10：代表redis键值(具体记录)
        if not tbls:
            return
        key = tbls[0].make_redis_index_key(tb_index)
        redis_cmd = "EXISTS " + key
        redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
        if redis_result == 1:   # 索引存在
            if is_all_cached:
                redis_cmd = "ZADD {} {} {}".format(key, DBHandler.INDEX_ALL_CACHED_SCORE, DBHandler.INDEX_ALL_CACHED)
            else:
                redis_cmd = "ZADD {}".format(key)
        else:
            all_cached_score = DBHandler.INDEX_ALL_CACHED_SCORE if is_all_cached else DBHandler.INDEX_NOT_ALL_CACHED_SCORE
            redis_cmd = "ZADD {} {} {}".format(key, all_cached_score, DBHandler.INDEX_ALL_CACHED)
        for tbl in tbls:
            redis_cmd += " {} {}".format(DBHandler.INDEX_PRI_KEY_SCORE, tbl.make_redis_pri_key())
        redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
        if redis_result is None:
            logger.log_error("exec redis cmd error, {}", redis_cmd)

    def _remove_cache_tbl_index_from_redis(self, tbls, tb_index):
        key = tbls[0].make_redis_index_key(tb_index)
        redis_cmd = "ZREM " + key
        for tbl in tbls:
            redis_cmd += " {}".format(tbl.make_redis_pri_key())
        redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
        if redis_result is None:
            logger.log_error("exec redis cmd error, {}", redis_cmd)

    def _load_from_db_and_cache(self, tbl):
        lst = self.get_row(tbl)
        self._cache_to_redis(lst)
        return lst

    def _cache_to_redis(self, tbls):
        for tbl in tbls:
            col_names = tbl._column_names
            pri_key = tbl.make_redis_pri_key()
            redis_cmd = "HSET " + pri_key
            for name in col_names:
                val = tbl[name]
                # todo: 怎么存入空字符串?
                if val == "":
                    redis_cmd += ' {} "{}"'.format(name, val)
                else:
                    redis_cmd += " {} {}".format(name, val)
            redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
            if redis_result is None:
                logger.log_error("exec redis cmd error, {}", redis_cmd)

    def _add_changed_tbls(self, tbls):
        redis_cmd = "SADD _chg_keys"
        for tbl in tbls:
            redis_cmd += " " + tbl.make_redis_pri_key()
        redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
        if redis_result is None:
            logger.log_error("exec redis cmd error, {}", redis_cmd)

    def _remove_changed_tbls(self, tbls):
        redis_cmd = "SREM _chg_keys"
        for tbl in tbls:
            redis_cmd += " " + tbl.make_redis_pri_key()
        redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
        if redis_result is None:
            logger.log_error("exec redis cmd error, {}", redis_cmd)

    def _on_flush_redis(self):
        self.flush_redis()

    def flush_add_clean_redis(self):
        self.flush_redis()
        self._db_redis.exec_redis_cmd("FLUSHDB")

    def flush_redis(self):
        logger.log_info("begin flush redis!!!!")
        redis_cmd = "SMEMBERS _chg_keys"
        redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
        if redis_result is None:
            logger.log_error("exec redis cmd error, {}", redis_cmd)

        flush_count = 0
        if len(redis_result) > 0:
            all_result = []
            for key in redis_result:
                tb_name = game.util.db_util.get_tbl_name_from_key(key)
                result = self._load_data_from_redis(tb_name, key)
                if not result:
                    logger.log_warn("not found redis key {}", key)
                    continue
                flush_count += len(result)
                all_result.extend(result)
                if len(all_result) >= 1000:
                    if not self.replace_rows(tuple(all_result)):
                        logger.log_error("flush redis error, update mysql failed, count: {}", len(all_result))
                        return
                    all_result = []

            if not all_result:
                if not self.replace_rows(tuple(all_result)):
                    logger.log_error("flush redis error, update mysql failed, count: {}", len(all_result))
                    return

            self._db_redis.exec_redis_cmd("DEL _chg_keys")
        logger.log_info("end flush redis!!!!, flush count: {}", flush_count)

    def insert_table(self, tbls):
        if type(tbls) not in (tuple, list):
            tbls = (tbls,)
        index_dict = {}
        for tbl in tbls:
            if tbl.primary_val is not None:
                res = self._load_data_by_primary_key(tbl, tbl.make_redis_pri_key())
                if res:
                    logger.log_info("insert error, key {} has exist", tbl.make_redis_pri_key())
                    return False
            tbl.init_with_default()
            for tb_index in tbl.all_index:
                if tb_index not in index_dict:
                    index_dict[tb_index] = []
                index_dict[tb_index].append(tbl)
        self._cache_to_redis(tbls)
        for tb_index, tbls in index_dict.items():
            self._cache_tbl_index_to_redis(tbls, tb_index, False)
        self._add_changed_tbls(tbls)
        return True

    def update_table(self, tbls):
        if type(tbls) not in (tuple, list):
            tbls = (tbls,)
        old_index = {}
        new_index = {}
        for tbl in tbls:
            if tbl.primary_val is None:
                logger.log_error("update table failed, not set primary key val, table:{}", tbl.tb_name)
                return False
            pri_key = tbl.make_redis_pri_key()
            res = self._load_data_by_primary_key(tbl, pri_key)
            redis_cmd = "HSET " + pri_key
            old_tbl = res[0] if res else None
            has_change = False
            for col_name in tbl.all_column_names:
                if tbl[col_name] is None:
                    continue
                if old_tbl is None or tbl[col_name] != old_tbl[col_name]:
                    has_change = True
                    redis_cmd += " {} {}".format(col_name, tbl[col_name])
                    # 更新索引
                    indexs = tbl.get_indexs_by_col(col_name)
                    for tb_index in indexs:
                        if old_tbl is not None:
                            if tb_index not in old_index:
                                old_index[tb_index] = []
                            if old_tbl not in old_index[tb_index]:
                                old_index[tb_index].append(old_tbl)
                        if tb_index not in new_index:
                            new_index[tb_index] = []
                        if tbl not in new_index[tb_index]:
                            new_index[tb_index].append(tbl)

            if has_change:
                redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
                if redis_result is None:
                    logger.log_error("exec redis cmd error, {}", redis_cmd)

        for tb_index, tbls in new_index.items():
            self._cache_tbl_index_to_redis(tbls, tb_index, False)

        for tb_index, tbls in old_index.items():
            self._remove_cache_tbl_index_from_redis(tbls, tb_index)

        self._add_changed_tbls(tbls)
        return True

    def delete_table(self, tbls):
        if type(tbls) not in (tuple, list):
            tbls = (tbls,)
        for tbl in tbls:
            primary_val = tbl.primary_val
            if primary_val is None:
                logger.log_error("delete table must use primary key, table:{}", tbl.tb_name)
                return False
        keys = []
        index_dict = {}
        for tbl in tbls:
            pri_key = tbl.make_redis_pri_key()
            keys.append(pri_key)
            res = self._load_data_from_redis(tbl.tb_name, pri_key)
            old_tbl = res[0] if res else None
            if old_tbl is not None:
                for tb_index in tbl.all_index:
                    if tb_index not in index_dict:
                        index_dict[tb_index] = []
                    index_dict[tb_index].append(old_tbl)
            self.delete_row(tbl)
        redis_cmd = "DEL " + " ".join(keys)
        redis_result = self._db_redis.exec_redis_cmd(redis_cmd)
        if redis_result is None:
            logger.log_error("exec redis cmd error, {}", redis_cmd)
        for tb_index, tbls in index_dict.items():
            self._remove_cache_tbl_index_from_redis(tbls, tb_index)
        self._remove_changed_tbls(tbls)
        return True

    @staticmethod
    def test_db():
        db_handler = DBHandler("test_db")
        # print(db_handler.name)
        # self._db_handler.test()

        db_handler.execute_sql("drop table if exists test")
        db_handler.execute_sql("create table if not exists test(id int, name varchar(50))")
        db_res = db_handler.execute_sql("insert into test values(1, 'test')")
        assert(db_res[0].update_count == 1)
        db_res = db_handler.execute_sql("select * from test")
        assert(len(db_res) == 1)
        db_res = db_handler.execute_sql("insert into test values(2, 'test1')")
        assert(db_res[0].update_count == 1)
        db_res = db_handler.execute_sql("select * from test")
        assert(len(db_res) == 2)
        for res in db_res:
            print(res.id, res.name)

        db_res = db_handler.execute_sql("select count(*) as count from test")
        assert(db_res[0].count == 2)

        # PyDb.executeSql(db_handler, "create table test(id int, name varchar(50))")
        # PyDb.initTable("player", ({"fieldName" : "id", "filedType" : 1},))
        #
        # tb_player = db.TbPlayer.TbPlayer()
        # print(tb_player.__dict__)
        # PyDb.createTable(db_handler, tb_player)

        print("db test success!!!")

    def test_db_and_redis(self):
        err_msg = "test db and redis failed!!!"

        def _output(flag):
            tbl_sql = game.util.db_util.create_tbl_obj("test")
            tbl_sql.role_id = 3
            res = self.load_tb_data(tbl_sql)
            print(flag)
            for tbl in res:
                tbl.dump_cols()
            print("\n")
            return res

        print("--------test insert--------")
        tbl_test = game.util.db_util.create_tbl_obj("test")
        tbl_test.role_id = 3
        tbl_test.role_name = "test3"
        tbl_test.account = "test3"
        self.insert_table(tbl_test)
        res = _output("after insert:")
        if len(res) != 1:
            raise RuntimeError(err_msg)
        res_tbl = res[0]
        if res_tbl.role_id != 3 or res_tbl.role_name != "test3" or res_tbl.account != "test3":
            raise RuntimeError(err_msg)

        print("--------test update--------")
        tbl_test = game.util.db_util.create_tbl_obj("test")
        tbl_test.role_id = 3
        tbl_test.role_name = "test3-1"
        self.update_table(tbl_test)
        res = _output("after update:")
        if len(res) != 1:
            raise RuntimeError(err_msg)
        res_tbl = res[0]
        if res_tbl.role_id != 3 or res_tbl.role_name != "test3-1" or res_tbl.account != "test3":
            raise RuntimeError(err_msg)

        print("--------test delete--------")
        tbl_test = game.util.db_util.create_tbl_obj("test")
        tbl_test.role_id = 3
        self.delete_table(tbl_test)
        res = _output("after delete:")
        if len(res) != 0:
            raise RuntimeError(err_msg)

        logger.log_info("db and redis test ok!!!")

    def redis_benchmark(self):
        import time

        from game.db.tbl.tbl_player import TblPlayer
        tbl = TblPlayer()
        tbl.role_id = 1

        st = time.time()
        for i in range(10000):
            res = self.load_tb_data(tbl)
        print("[redis cache] load time=", time.time() - st)
        print(res)

        st = time.time()
        for i in range(10000):
            res = self._db_redis.exec_redis_cmd("HGETALL player:1")
        print("[redis only] load time=", time.time() - st)
        print(res)

        st = time.time()
        for i in range(10000):
            res = self.get_row(tbl)
        print("[sql] load time=", time.time() - st)
        print(res)

        st = time.time()
        for i in range(10000):
            res = self.execute_sql("select * from player where role_id=1")
        print("[sql only] load time=", time.time() - st)
        print(res)
