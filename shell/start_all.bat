@ECHO OFF

call config.bat

set RouterConfig="../../conf/router.cfg"
set ServerConfig="../../conf/scene.cfg"
set GatewayConfig="../../conf/gateway.cfg"
set DBConfig="../../conf/db.cfg"

cd ../bin/Debug
start %RouterCmd% %RouterConfig%
start %ServerCmd% %DBConfig%
start %ServerCmd% %ServerConfig%
start %GatewayCmd% %GatewayConfig%

::EXIT