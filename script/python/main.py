#-*- ecoding:utf-8 -*-
import os
import sys
import db.TbMgr
import py_cffi.cffi_test

def init():
    print("init python", os.getcwd(), os.path.abspath(""))
    # import Test
    # print(Test.test())
    # db.TbMgr.TbMgr.initTbTable()
    # cffi_test.test1()
    py_cffi.cffi_test.test_use()
    print("init python end")

