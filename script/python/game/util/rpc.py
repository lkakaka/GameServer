
from proto.pb_message import Message
import Timer
import weakref
import logger


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
        self._timer_id = Timer.addTimer(int(self._time_out * 1000), 0, 0, self._on_future_timeout)
        self.finish_cb = _FutureCallback()
        self.timeout_cb = _FutureCallback()

    def on_recv_rsp(self, rpc_data):
        self.remove_timeout_timer()
        for fin_cb in self.finish_cb.future_cb:
            fin_cb(rpc_data)
        logger.logInfo("$future finish, rpc_id:{}", self._rpc_id)

    def _on_future_timeout(self):
        self._rpc_mgr().remove_future(self._rpc_id)
        for tt_cb in self.timeout_cb.future_cb:
            tt_cb()
        logger.logInfo("$future timeout, rpc_id:{}", self._rpc_id)

    def remove_timeout_timer(self):
        if self._timer_id < 0:
            return
        Timer.removeTimer(self._timer_id)
        self._timer_id = -1


class RpcMgr(object):

    DEFAULT_TIME_OUT = 30  # Ãë

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
        logger.logInfo("$add future, rpc_id:{}, time_out:{}, timer_id:{}", rpc_id, time_out, future._timer_id)
        return future

    def remove_future(self, rpc_id):
        self._futures.pop(rpc_id, None)

    def rpc_call(self, service_name, rpc_func_name, rpc_func_params, time_out=DEFAULT_TIME_OUT, **kwargs):
        rpc_msg = Message.create_msg_by_id(Message.MSG_ID_RPC_MSG)
        rpc_id = self.alloc_rpc_id()
        rpc_msg.rpc_id = rpc_id
        rpc_msg.rpc_func = rpc_func_name
        # rpc_msg.rpc_param = rpc_func_params
        rpc_msg.rpc_param = repr(kwargs)
        future = self._add_future(rpc_id, time_out)
        self.service.send_msg_to_service(service_name, Message.MSG_ID_RPC_MSG, rpc_msg)
        return future

    def on_recv_rpc_rsp_msg(self, sender, msg):
        future = self._futures.pop(msg.rpc_id, None)
        if future is None:
            logger.logError("$not found rpc futurue, rpc_id:{}", msg.rpc_id)
            return
        future.on_recv_rsp(msg.rpc_data)
