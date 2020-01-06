#-*- ecoding:utf-8 -*-
import os
import sys
import db.TbMgr
import logger
import Timer
import py_cffi.cffi_test
import service.db_service
import service.scene_service
from proto.message import message
import Game

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
    timer_id = Timer.addTimer(3000, 1000, 3, timer_cb)
    logger.logInfo("$add timer {}", timer_id)


def create_db_service():
    service_inst = service.db_service.DBService()
    service_inst.on_service_start()
    return service_inst

def create_scene_service():
    service_inst = service.scene_service.SceneService()
    service_inst.on_service_start()
    return service_inst

def on_recv_client_msg(service, conn_id, msg_id, msg_data):
    print("on_recv_client_msg---", service, conn_id, msg_id, msg_data)
    msg = message.create_msg_by_id(msg_id)
    msg.ParseFromString(msg_data)
    if msg_id == message.MSG_ID_LOGIN:
        print("recv login---", msg.account, msg.pwd)
        rsp_msg = message.create_msg_by_id(message.MSG_ID_LOGINRSP)
        rsp_msg.account = "test"
        rsp_msg.user_id = 1
        rsp_str = rsp_msg.SerializeToString()
        print("login rsp===", rsp_str)
        Game.sendMessage(conn_id, message.MSG_ID_LOGINRSP, rsp_str)
    elif msg_id == message.MSG_ID_TEST:
        rsp_msg = message.create_msg_by_id(message.MSG_ID_TEST)
        rsp_msg.id = 10
        rsp_msg.msg = "hello"
        rsp_str = rsp_msg.SerializeToString()
        Game.sendMessage(conn_id, message.MSG_ID_TEST, rsp_str)
    # service.on_recv_msg(sender, msg)
    # import proto.test_pb2
    # test = proto.test_pb2.Test()
    # test.ParseFromString(msg)
    # print("test proto", test.id, test.msg)
    # import proto.login_pb2
    # login_rsp = proto.login_pb2.LoginRsp()
    # login_rsp.account = "py_test"
    # login_rsp.user_id = 11
    # print("on_recv_client_msg---login rsp", login_rsp.account)


def on_recv_service_msg(service, sender, msgId, msg):
    print("on_recv_service_msg---", service, sender, msgId, msg)


def timer_cb():
    print("timer cb called")
    # Timer.removeTimer(timer_id)

