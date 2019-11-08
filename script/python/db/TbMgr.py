#-*- ecoding:utf-8 -*-
import PyDb

class TbMgr:
    @staticmethod
    def initTbTable():
        print("start init tb table")
        PyDb.initTable("player", ({"fieldName" : "id".encode("utf8"), "filedType" : 1},))

