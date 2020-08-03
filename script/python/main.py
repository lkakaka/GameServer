# -*- encoding:utf-8 -*-

import os
from game.util import logger
import game.util.db_util
import Timer
import platform
import builtins
println = builtins.print

is_win = platform.platform().find("Windows") >= 0
def print_1(*args, **kwargs):
    if is_win:
        println(*args, **kwargs)

builtins.print = print_1

timer_id = 0


def init():
    logger.log_info("init python {} {}", os.getcwd(), os.path.abspath(""))
    # import Test
    # print(Test.test())
    # py_cffi.cffi_test.test_use()
    # test_timer()
    logger.log_info("init python end")


def thread_test():
    # print("thread test")
    logger.log_info("python thread test")


def test_timer():
    global timer_id

    def timer_cb():
        print("timer cb called")

    timer_id = Timer.addTimer(3000, 1000, 3, timer_cb)
    logger.log_info("add timer {}", timer_id)


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


def create_tb(tb_name):
    # print("create tb --------", tb_name)
    return game.util.db_util.create_tbl_obj(tb_name)


import hotfix.hotfix
hotfix.hotfix.on_server_start()

# def timer_cb():
#     print("timer cb called")
#     # Timer.removeTimer(timer_id)
