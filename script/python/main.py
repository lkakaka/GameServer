#-*- ecoding:utf-8 -*-
import os
import db.TbMgr

def init():
    print("init python", os.getcwd())
    import Test
    print(Test.test())
    db.TbMgr.TbMgr.initTbTable()
    print("init python end")
