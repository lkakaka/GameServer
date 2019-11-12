#-*- ecoding:utf-8 -*-
import os
import sys
import db.TbMgr
import logger
import py_cffi.cffi_test

def init():
    logger.logInfo("$init python {} {}", os.getcwd(), os.path.abspath(""))
    # import Test
    # print(Test.test())
    # db.TbMgr.TbMgr.initTbTable()
    py_cffi.cffi_test.test_use()
    logger.logInfo("$init python end")

def thread_test():
    # print("thread test")
    logger.logInfo("$python thread test")
