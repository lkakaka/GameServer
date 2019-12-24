@ECHO OFF

call config.bat

set RouterConfig="../../conf/router.cfg"
set ServerConfig="../../conf/scene.cfg"
set GatewayConfig="../../conf/gateway.cfg"

cd ../bin/Debug
start %RouterCmd% %RouterConfig%
start %ServerCmd% %ServerConfig%
start %GatewayCmd% %GatewayConfig%

::EXIT