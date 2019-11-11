#-*- ecoding:utf-8 -*-
import os
# import db.TbMgr
import cffi_test

def init():
    print("init python", os.getcwd())
    # import Test
    # print(Test.test())
    # db.TbMgr.TbMgr.initTbTable()
    cffi_test.test1()
    cffi_test.test_use()
    print("init python end")

init()
