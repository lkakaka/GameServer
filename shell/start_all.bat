@echo off

call config.bat

::cd ../bin/Debug
start %RouterCmd% %RouterConfig%
start %ServerCmd% %DBConfig%
start %ServerCmd% %SceneConfig%
start %GatewayCmd% %GatewayConfig%

::EXIT