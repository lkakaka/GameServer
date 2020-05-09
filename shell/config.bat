
set version=release

if %version%==debug (
#debug
set ExePath=..\bin\Debug\
set RouterExe=ZmqRouter_d.exe
set GatewayExe=Gateway_d.exe
set GameServerExe=GameServer_d.exe
) else ( 
#release
set ExePath=..\bin\Release\
set RouterExe=ZmqRouter.exe
set GatewayExe=Gateway.exe
set GameServerExe=GameServer.exe
)

set RouterCmd=%ExePath%%RouterExe%
set GatewayCmd=%ExePath%%GatewayExe%
set ServerCmd=%ExePath%%GameServerExe%

set RouterConfig="../conf/router.cfg"
set SceneCtrlConfig="../conf/scene_ctrl.cfg"
set SceneConfig="../conf/scene.cfg"
set GatewayConfig="../conf/gateway.cfg"
set DBConfig="../conf/db.cfg"
set LoginConfig="../conf/login.cfg"
