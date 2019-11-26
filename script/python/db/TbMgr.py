#-*- ecoding:utf-8 -*-
import PyDb
import db.TbPlayer

class TbMgr:
    @staticmethod
    def initTbTable():
        print("start init tb table")
        db_handler = PyDb.createDB("test3")
        # PyDb.executeSql(db_handler, "create table test(id int, name varchar(50))")
        # PyDb.initTable("player", ({"fieldName" : "id".encode("utf8"), "filedType" : 1},))

        PyDb.createTable(db_handler, db.TbPlayer.TbPlayer())


