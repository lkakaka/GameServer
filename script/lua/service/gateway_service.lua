require("base.object")
require("service.service_base")
require("base.service_type")
require("proto.pb_message")
require("util.logger")
require("util.const")
require("util.str_util")
require("util.timer")

clsGatewayService = clsServiceBase:Inherit("clsGatewayService")

function clsGatewayService:__init__()
    Super(clsGatewayService).__init__(self)
    timer.add_timer(3, 1, function(timer_id) self:reg_port_to_scene_ctrl() end)
end

function clsGatewayService:reg_port_to_scene_ctrl()
    local port = Config:getConfigInt("port")
    local udp_port = Config:getConfigInt("udp_port")
    local port_info = { port = port, udp_port = udp_port }
    local future = self:callRpc(LOCAL_SERVICE_SCENE_CTRL, "RegGatewayPort", 10, port_info)
    future:regCallback(function(err_code, result) self:on_reg_port(err_code) end)
end

function clsGatewayService:on_reg_port(err_code)
    if err_code == ErrorCode.OK then
        logger.logInfo("reg port success!!!")
        return
    end
    timer.add_timer(3, 1, function(timer_id) self:reg_port_to_scene_ctrl() end)
    logger.logInfo("reg port faild, retry reg!!!")
end