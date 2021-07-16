require("base.object")
require("service.service_base")
require("base.service_type")
require("proto.pb_message")

clsLoginService = clsServiceBase:Inherit("clsLoginService")

function clsLoginService:__init__()
    Super(clsLoginService).__init__(self)
    print("clsLoginService:__init__")
end

function clsLoginService:initClientMsgHandler()
    self:regClientMsgHandler(MSG_ID_LOGIN_REQ, self.loginReq)
end

function clsLoginService:loginReq(conn_id, msg_id, msg)
    
end
