require("base.object")
require("util.logger")

-- local pb = require("pb") -- 载入 pb.dll

-- -- assert(pb.loadfile "E:\\project\\MyServer\\script\\lua\\pb\\common.pb") -- 载入刚才编译的pb文件
-- assert(pb.loadfile("..\\script\\lua\\pb\\common.pb")) -- 载入刚才编译的pb文件
-- assert(pb.loadfile("..\\script\\lua\\pb\\login.pb")) -- 载入刚才编译的pb文件

clsServiceBase = clsObject:Inherit("clsServiceBase")

function clsServiceBase:__init__()
    self._msg_handler = {}
end

function clsServiceBase:regMsgHandler(msgId, handler)
    self._msg_hander[msgId] = handler
end

function clsServiceBase:on_recv_service_msg(sender, msgId, msg)
    print(sender, msgId, msg)
    logger.log_info("on_recv_service_msg")

    local person = { -- 我们定义一个addressbook里的 Person 消息
        name = "Alice",
        id = 12345,
        phone = {
            { number = "1301234567" },
            { number = "87654321", type = "WORK" },
        }
    }

    -- -- 序列化成二进制数据
    -- local data = assert(pb.encode("tutorial.Person", person))

    -- 从二进制数据解析出实际消息
    if msgId == 18 then
        print("decode msg")
        local msg = assert(pb.decode("RpcMsg", msg))
        for k, v in pairs(msg) do
            print(k, v)
        end
    end

    local handler = self._msg_hander[msgId]
    if handler == nil then
        logger.log_error("not found msg handler, msgId:" .. msgId)
        return
    end

    local pbMsg = decodeMsg(msgId, msg)
    handler(sender, msgId, pbMsg)
end

function clsServiceBase:on_recv_client_msg(sender, msgId, msg)
    logger.log_info("on_recv_client_msg")
end