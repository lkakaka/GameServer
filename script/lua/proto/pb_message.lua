
-- Generated by tool.  DO NOT EDIT!
require("util.logger")
pb = require("pb") -- 载入 pb.dll

assert(pb.loadfile("../script/lua/proto/scene.pb")) -- 载入pb文件
assert(pb.loadfile("../script/lua/proto/server_only/login_srv.pb")) -- 载入pb文件
assert(pb.loadfile("../script/lua/proto/login.pb")) -- 载入pb文件
assert(pb.loadfile("../script/lua/proto/role.pb")) -- 载入pb文件
assert(pb.loadfile("../script/lua/proto/server_only/common.pb")) -- 载入pb文件
assert(pb.loadfile("../script/lua/proto/test.pb")) -- 载入pb文件

MSG_ID_ACTOR_BORN = 1
MSG_ID_ACTOR_DISSOLVE = 2
MSG_ID_CLIENT_DISCONNECT = 3
MSG_ID_CREATE_ROLE_REQ = 4
MSG_ID_CREATE_ROLE_RSP = 5
MSG_ID_DISCONNECT_REQ = 6
MSG_ID_DISCONNECT_RSP = 7
MSG_ID_ENTER_GAME = 8
MSG_ID_ENTER_GAME_RSP = 9
MSG_ID_ENTER_SCENE_REQ = 10
MSG_ID_ENTER_SCENE_RSP = 11
MSG_ID_GM_CMD = 12
MSG_ID_GM_CMD_RSP = 13
MSG_ID_LOAD_ROLE_LIST_RSP = 14
MSG_ID_LOGIN_REQ = 15
MSG_ID_LOGIN_RSP = 16
MSG_ID_MOVE_TO = 17
MSG_ID_RPC_MSG = 18
MSG_ID_RPC_MSG_RSP = 19
MSG_ID_SWITCH_SCENE_SERVICE = 20
MSG_ID_SYNC_POS = 21
MSG_ID_TEST_REQ = 22

MSG_ID_TO_NAME = {
	[1] = "ActorBorn",
	[2] = "ActorDissolve",
	[3] = "ClientDisconnect",
	[4] = "CreateRoleReq",
	[5] = "CreateRoleRsp",
	[6] = "DisconnectReq",
	[7] = "DisconnectRsp",
	[8] = "EnterGame",
	[9] = "EnterGameRsp",
	[10] = "EnterSceneReq",
	[11] = "EnterSceneRsp",
	[12] = "GmCmd",
	[13] = "GmCmdRsp",
	[14] = "LoadRoleListRsp",
	[15] = "LoginReq",
	[16] = "LoginRsp",
	[17] = "MoveTo",
	[18] = "RpcMsg",
	[19] = "RpcMsgRsp",
	[20] = "SwitchSceneService",
	[21] = "SyncPos",
	[22] = "TestReq",
}

MSG_NAME_TO_ID = {
	ActorBorn = 1,
	ActorDissolve = 2,
	ClientDisconnect = 3,
	CreateRoleReq = 4,
	CreateRoleRsp = 5,
	DisconnectReq = 6,
	DisconnectRsp = 7,
	EnterGame = 8,
	EnterGameRsp = 9,
	EnterSceneReq = 10,
	EnterSceneRsp = 11,
	GmCmd = 12,
	GmCmdRsp = 13,
	LoadRoleListRsp = 14,
	LoginReq = 15,
	LoginRsp = 16,
	MoveTo = 17,
	RpcMsg = 18,
	RpcMsgRsp = 19,
	SwitchSceneService = 20,
	SyncPos = 21,
	TestReq = 22,
}

local function decodeActorBorn(msg)
	return pb.decode("ActorBorn", msg)
end

local function decodeActorDissolve(msg)
	return pb.decode("ActorDissolve", msg)
end

local function decodeClientDisconnect(msg)
	return pb.decode("ClientDisconnect", msg)
end

local function decodeCreateRoleReq(msg)
	return pb.decode("CreateRoleReq", msg)
end

local function decodeCreateRoleRsp(msg)
	return pb.decode("CreateRoleRsp", msg)
end

local function decodeDisconnectReq(msg)
	return pb.decode("DisconnectReq", msg)
end

local function decodeDisconnectRsp(msg)
	return pb.decode("DisconnectRsp", msg)
end

local function decodeEnterGame(msg)
	return pb.decode("EnterGame", msg)
end

local function decodeEnterGameRsp(msg)
	return pb.decode("EnterGameRsp", msg)
end

local function decodeEnterSceneReq(msg)
	return pb.decode("EnterSceneReq", msg)
end

local function decodeEnterSceneRsp(msg)
	return pb.decode("EnterSceneRsp", msg)
end

local function decodeGmCmd(msg)
	return pb.decode("GmCmd", msg)
end

local function decodeGmCmdRsp(msg)
	return pb.decode("GmCmdRsp", msg)
end

local function decodeLoadRoleListRsp(msg)
	return pb.decode("LoadRoleListRsp", msg)
end

local function decodeLoginReq(msg)
	return pb.decode("LoginReq", msg)
end

local function decodeLoginRsp(msg)
	return pb.decode("LoginRsp", msg)
end

local function decodeMoveTo(msg)
	return pb.decode("MoveTo", msg)
end

local function decodeRpcMsg(msg)
	return pb.decode("RpcMsg", msg)
end

local function decodeRpcMsgRsp(msg)
	return pb.decode("RpcMsgRsp", msg)
end

local function decodeSwitchSceneService(msg)
	return pb.decode("SwitchSceneService", msg)
end

local function decodeSyncPos(msg)
	return pb.decode("SyncPos", msg)
end

local function decodeTestReq(msg)
	return pb.decode("TestReq", msg)
end

	

local decodeFunc = {
	[1] = decodeActorBorn,
	[2] = decodeActorDissolve,
	[3] = decodeClientDisconnect,
	[4] = decodeCreateRoleReq,
	[5] = decodeCreateRoleRsp,
	[6] = decodeDisconnectReq,
	[7] = decodeDisconnectRsp,
	[8] = decodeEnterGame,
	[9] = decodeEnterGameRsp,
	[10] = decodeEnterSceneReq,
	[11] = decodeEnterSceneRsp,
	[12] = decodeGmCmd,
	[13] = decodeGmCmdRsp,
	[14] = decodeLoadRoleListRsp,
	[15] = decodeLoginReq,
	[16] = decodeLoginRsp,
	[17] = decodeMoveTo,
	[18] = decodeRpcMsg,
	[19] = decodeRpcMsgRsp,
	[20] = decodeSwitchSceneService,
	[21] = decodeSyncPos,
	[22] = decodeTestReq,
}

local function encodeActorBorn(msg)
	return pb.encode("ActorBorn", msg)
end

local function encodeActorDissolve(msg)
	return pb.encode("ActorDissolve", msg)
end

local function encodeClientDisconnect(msg)
	return pb.encode("ClientDisconnect", msg)
end

local function encodeCreateRoleReq(msg)
	return pb.encode("CreateRoleReq", msg)
end

local function encodeCreateRoleRsp(msg)
	return pb.encode("CreateRoleRsp", msg)
end

local function encodeDisconnectReq(msg)
	return pb.encode("DisconnectReq", msg)
end

local function encodeDisconnectRsp(msg)
	return pb.encode("DisconnectRsp", msg)
end

local function encodeEnterGame(msg)
	return pb.encode("EnterGame", msg)
end

local function encodeEnterGameRsp(msg)
	return pb.encode("EnterGameRsp", msg)
end

local function encodeEnterSceneReq(msg)
	return pb.encode("EnterSceneReq", msg)
end

local function encodeEnterSceneRsp(msg)
	return pb.encode("EnterSceneRsp", msg)
end

local function encodeGmCmd(msg)
	return pb.encode("GmCmd", msg)
end

local function encodeGmCmdRsp(msg)
	return pb.encode("GmCmdRsp", msg)
end

local function encodeLoadRoleListRsp(msg)
	return pb.encode("LoadRoleListRsp", msg)
end

local function encodeLoginReq(msg)
	return pb.encode("LoginReq", msg)
end

local function encodeLoginRsp(msg)
	return pb.encode("LoginRsp", msg)
end

local function encodeMoveTo(msg)
	return pb.encode("MoveTo", msg)
end

local function encodeRpcMsg(msg)
	return pb.encode("RpcMsg", msg)
end

local function encodeRpcMsgRsp(msg)
	return pb.encode("RpcMsgRsp", msg)
end

local function encodeSwitchSceneService(msg)
	return pb.encode("SwitchSceneService", msg)
end

local function encodeSyncPos(msg)
	return pb.encode("SyncPos", msg)
end

local function encodeTestReq(msg)
	return pb.encode("TestReq", msg)
end

	

local encodeFunc = {
	[1] = encodeActorBorn,
	[2] = encodeActorDissolve,
	[3] = encodeClientDisconnect,
	[4] = encodeCreateRoleReq,
	[5] = encodeCreateRoleRsp,
	[6] = encodeDisconnectReq,
	[7] = encodeDisconnectRsp,
	[8] = encodeEnterGame,
	[9] = encodeEnterGameRsp,
	[10] = encodeEnterSceneReq,
	[11] = encodeEnterSceneRsp,
	[12] = encodeGmCmd,
	[13] = encodeGmCmdRsp,
	[14] = encodeLoadRoleListRsp,
	[15] = encodeLoginReq,
	[16] = encodeLoginRsp,
	[17] = encodeMoveTo,
	[18] = encodeRpcMsg,
	[19] = encodeRpcMsgRsp,
	[20] = encodeSwitchSceneService,
	[21] = encodeSyncPos,
	[22] = encodeTestReq,
}

function decodeMsg(msgId, msg)
	local func = decodeFunc[msgId]
	if func == nil then
		logger.logError("decode unkown msgId:" .. msgId)
		return
	end
	return func(msg);
end

function encodeMsg(msgId, msg)
	local func = encodeFunc[msgId]
	if func == nil then
		logger.logError("encode unkown msgId:" .. msgId)
		return
	end
	return func(msg);
end