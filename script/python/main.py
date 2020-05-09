#-*- ecoding:utf-8 -*-
import os
import sys
# import db.TbMgr
import logger
import Timer
import py_cffi.cffi_test

timer_id = 0

def init():
    logger.logInfo("$init python {} {}", os.getcwd(), os.path.abspath(""))
    # import Test
    # print(Test.test())
    db.TbMgr.TbMgr.initTbTable()
    # py_cffi.cffi_test.test_use()
    # test_timer()
    logger.logInfo("$init python end")

def thread_test():
    # print("thread test")
    logger.logInfo("$python thread test")

def test_timer():
    global timer_id

    def timer_cb():
        print("timer cb called")

    timer_id = Timer.addTimer(3000, 1000, 3, timer_cb)
    logger.logInfo("$add timer {}", timer_id)


def create_db_service():
    import game.service.db_service
    service_inst = game.service.db_service.DBService()
    service_inst.on_service_start()
    return service_inst


def create_scene_service():
    import game.service.scene_service
    service_inst = game.service.scene_service.SceneService()
    service_inst.on_service_start()
    # test_timer()
    return service_inst


def create_scene_ctrl_service():
    import game.service.scene_ctrl_service
    service_inst = game.service.scene_ctrl_service.SceneCtrlService()
    service_inst.on_service_start()
    return service_inst
    

def create_login_service():
    import game.service.login_service
    service_inst = game.service.login_service.LoginService()
    service_inst.on_service_start()
    return service_inst


# def timer_cb():
#     print("timer cb called")
#     # Timer.removeTimer(timer_id)

