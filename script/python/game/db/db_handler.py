
import PyDb


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
    def __init__(self, db_name):
        self._db_name = db_name
        self._db_inst = PyDb.DbInst(self._db_name)
        assert(self._db_inst is not None)

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
        self._db_inst.insert_row(tbl)

    def update_row(self, tbl):
        self._db_inst.updateRow(tbl)

    def get_row(self, tbl):
        self._db_inst.getRow(tbl)

    def delete_row(self, tbl):
        self._db_inst.delete_row(tbl)

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

