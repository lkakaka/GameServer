
í
proto/login.proto"O
LoginReq
account (	Raccount
pwd (	Rpwd
conn_id (RconnId"%
LoginRsp
err_code (RerrCode"A
	_RoleInfo
role_id (RroleId
	role_name (	RroleName"o
LoadRoleListRsp
account (	Raccount
err_code (RerrCode'
	role_list (2
._RoleInfoRroleList"F
CreateRoleReq
account (	Raccount
	role_name (	RroleName"S
CreateRoleRsp
err_code (RerrCode'
	role_info (2
._RoleInfoRroleInfo">
	EnterGame
role_id (RroleId
account (	Raccount"R
EnterGameRsp
err_code (RerrCode'
	role_info (2
._RoleInfoRroleInfo"
DisconnectReq"'
DisconnectRsp
reason (	Rreason"7
StartKcp
kcp_id (RkcpId
token (	Rtoken"©
SwitchRemoteServer
role_id (RroleId
	remote_ip (	RremoteIp
remote_port (R
remotePort&
remote_udp_port (RremoteUdpPort
token (	Rtoken"@
RemoteEnterGame
role_id (RroleId
token (	RtokenB
	com.protoBLoginbproto3