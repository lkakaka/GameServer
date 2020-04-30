@echo off

call config.bat

::cd ../bin/Debug
start "Router-"%RouterCmd% %RouterCmd% %RouterConfig%
start "DBServer-"%ServerCmd% %ServerCmd% %DBConfig%
start "SceneServer-"%ServerCmd% %ServerCmd% %SceneConfig%
start "Gateway-"%GatewayCmd% %GatewayCmd% %GatewayConfig%

::EXIT