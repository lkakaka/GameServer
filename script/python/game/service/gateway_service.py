# -*- encoding:utf-8 -*-

from game.service.service_base import ServiceBase
from game.service.service_addr import LOCAL_SCENE_CTRL_SERVICE_ADDR
from game.util.const import ErrorCode
from game.util import logger
import game.util.cmd_util
import Config
import game.util.timer


class GatewayService(ServiceBase):
    _s_cmd = game.util.cmd_util.CmdDispatch("scene_ctrl_service")
    _rpc_proc = game.util.cmd_util.CmdDispatch("rpc_gateway_service")

    def __init__(self):
        ServiceBase.on_service_start(self)
        ServiceBase.__init__(self, GatewayService._s_cmd, None, GatewayService._rpc_proc)
        game.util.timer.add_timer(3, lambda: self.reg_addr_to_scene_ctrl())

    def on_service_start(self):
        logger.log_info("Gateway Service Start!!")

    def reg_addr_to_scene_ctrl(self):
        ip = Config.getConfigStr("ip")
        port = Config.getConfigInt("port")
        udp_port = Config.getConfigInt("udp_port")
        future = self.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "RegGatewayAddress", 10, ip=ip, port=port, udp_port=udp_port)
        future.on_fin += self.on_reg_addr
        future.on_timeout += self.on_reg_addr

    def on_reg_addr(self, err_code):
        if err_code == ErrorCode.OK:
            logger.log_info("reg addr success!!!")
            return
        game.util.timer.add_timer(3, lambda: self.reg_addr_to_scene_ctrl())
        logger.log_info("reg addr faild, retry!!!")
