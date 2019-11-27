#-*- ecoding:utf-8 -*-
import PyDb
import db.TbPlayer

class TbMgr:
    @staticmethod
    def initTbTable():
        print("start init tb table")
        db_handler = PyDb.createDB("test3")
        # PyDb.executeSql(db_handler, "create table test(id int, name varchar(50))")
        PyDb.initTable("player", ({"fieldName" : "id", "filedType" : 1},))

        tb_player = db.TbPlayer.TbPlayer()
        print(tb_player.__dict__)
        PyDb.createTable(db_handler, tb_player)


