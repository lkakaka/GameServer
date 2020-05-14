@echo off

call config.bat

::cd ../bin/Debug
start "Router-"%ServerCmd% %ServerCmd% %RouterConfig%
start "DBServer-"%ServerCmd% %ServerCmd% %DBConfig%
start "SceneCtrlServer-"%ServerCmd% %ServerCmd% %SceneCtrlConfig%
start "SceneServer-"%ServerCmd% %ServerCmd% %SceneConfig%
start "LoginServer-"%ServerCmd% %ServerCmd% %LoginConfig%
start "Gateway-"%ServerCmd% %ServerCmd% %GatewayConfig%

::EXIT