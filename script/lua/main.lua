require("service.login_service")
require("service.db_service")
require("service.scene_ctrl_service")
require("service.scene_service")
require("util.logger")
require("proto.pb_message")
require("util.timer")
require("base.cmd_line")
require("base.mem")

service_factory = {}

-- local str = "Hello, Lua !"
-- print(str)
-- local tab = {name = "hans", id = 123456}

-- logger.logInfo("$aaaa")

-- local ip = Config:getConfigStr("center_service_ip")
-- print(ip)

-- timer.test()

-- function add(x, y)
-- 	return x + y
-- end

function got_problems(error_msg)
	-- print(debug.traceback())
	return error_msg .. "\n" .. debug.traceback(nil, 2)
end

function service_factory.create_login_service()
	print("create_login_service")
	login_service = clsLoginService:New()
	-- assert(false, "assert")
	print("create_login_service end")
	return login_service
end

function service_factory.create_db_service()
	db_service = clsDBService:New()
	return db_service
end

function service_factory.create_scene_ctrl_service()
	scene_ctrl_service = clsSceneCtrlService:New()
	return scene_ctrl_service
end

function service_factory.create_scene_service()
	scene_service = clsSceneService:New()
	return scene_service
end

-- timer.add_timer(300, -1, Mem.gc())

-- for k, v in pairs(package.loaded) do
-- 	print(k, v)
-- end

-- for k, v in pairs(_G) do
-- 	if k == "Config" or k == "_Config" then
-- 		print(type(k), k, v)
-- 	end
-- 	-- if k == "main" then
-- 	-- 	for k1, v1 in pairs(v) do
-- 	-- 		print(k1, v1)
-- 	-- 	end
-- 	-- end
-- end