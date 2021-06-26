
set version=debug

if %version%==debug (
#debug
set ExePath=..\bin\Debug\
set GameServerExe=GameServer_d.exe
) else ( 
#release
set ExePath=..\bin\Release\
set GameServerExe=GameServer.exe
)

set ServerCmd=%ExePath%%GameServerExe%

set RouterConfig="./conf/router.cfg"
set SceneCtrlConfig="./conf/scene_ctrl.cfg"
set SceneConfig="./conf/scene.cfg"
set Scene1Config="./conf/scene1.cfg"
set GatewayConfig="./conf/gateway.cfg"
set DBConfig="./conf/db.cfg"
set LoginConfig="./conf/login.cfg"
