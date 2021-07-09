require("service.login_service")
require("service.db_service")
require("service.scene_ctrl_service")
require("service.scene_service")
require("util.logger")
require("proto.pb_message")
require("util.timer")

service_factory = {}

str = "Hello, Lua !"
print(str)
table = {name = "hans", id = 123456}

logger.log_info("$aaaa")

-- timer.test()

function add(x, y)
	return x + y
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

-- for k, v in pairs(_G) do
-- 	print(type(k), k, v)
-- 	-- if k == "main" then
-- 	-- 	for k1, v1 in pairs(v) do
-- 	-- 		print(k1, v1)
-- 	-- 	end
-- 	-- end
-- end