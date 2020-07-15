# -*- encoding:utf-8 -*-

import weakref

from proto.pb_message import Message
from game.util import logger
import game.util.const
import game.util.timer


class _FutureCallback(object):

    def __init__(self):
        self.future_cb = []

    def __add__(self, cb):
        self.future_cb.append(cb)
        return self


class _Future(object):

    def __init__(self, rpc_mgr, rpc_id, time_out):
        self._rpc_mgr = weakref.ref(rpc_mgr)
        self._rpc_id = rpc_id
        self._time_out = RpcMgr.DEFAULT_TIME_OUT if time_out <= 0 else time_out
        self._timer_id = game.util.timer.add_timer(self._time_out, self._on_future_timeout)
        self.on_fin = _FutureCallback()
        self.on_timeout = _FutureCallback()

    def on_recv_rsp(self, rpc_data):
        self.remove_timeout_timer()
        params = eval(rpc_data) if rpc_data != "" else ()
        for fin_cb in self.on_fin.future_cb:
            fin_cb(*params)
        logger.log_info("future finish, rpc_id:{}", self._rpc_id)

    def _on_future_timeout(self):
        self._rpc_mgr().remove_future(self._rpc_id)
        for tt_cb in self.on_timeout.future_cb:
            tt_cb(game.util.const.ErrorCode.TIME_OUT)
        logger.log_info("future timeout, rpc_id:{}", self._rpc_id)

    def remove_timeout_timer(self):
        if self._timer_id < 0:
            return
        game.util.timer.remove_timer(self._timer_id)
        self._timer_id = -1


class RpcMgr(object):

    DEFAULT_TIME_OUT = 30  # 绉�

    def __init__(self, service_obj):
        self._service_obj = weakref.ref(service_obj)
        self._max_rpc_id = 0
        self._futures = {}

    @property
    def service(self):
        return self._service_obj()

    def alloc_rpc_id(self):
        self._max_rpc_id += 1
        return self._max_rpc_id

    def _add_future(self, rpc_id, time_out):
        future = _Future(self, rpc_id, time_out)
        self._futures[rpc_id] = future
        logger.log_info("add future, rpc_id:{}, time_out:{}, timer_id:{}", rpc_id, time_out, future._timer_id)
        return future

    def remove_future(self, rpc_id):
        self._futures.pop(rpc_id, None)

    def rpc_call(self, service_name, rpc_func_name, time_out=DEFAULT_TIME_OUT, **kwargs):
        rpc_msg = Message.create_msg_by_id(Message.MSG_ID_RPC_MSG)
        rpc_id = self.alloc_rpc_id()
        rpc_msg.rpc_id = rpc_id
        rpc_msg.rpc_func = rpc_func_name
        rpc_msg.rpc_param = repr(kwargs)
        future = self._add_future(rpc_id, time_out)
        self.service.send_msg_to_service(service_name, rpc_msg)
        return future

    def on_recv_rpc_rsp_msg(self, sender, rpc_id, rpc_data):
        future = self._futures.pop(rpc_id, None)
        if future is None:
            logger.log_error("not found rpc futurue, rpc_id:{}", rpc_id)
            return
        future.on_recv_rsp(rpc_data)

