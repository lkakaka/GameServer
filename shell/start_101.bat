@echo off

call start.bat 101 all

::call start.bat 101 center
::call start.bat 101 db
::call start.bat 101 scene_ctrl
::call start.bat 101 login
::call start.bat 101 scene
::call start.bat 101 gateway

::call config.bat

::cd ../bin/Debug
::start "CenterServer-"%ServerCmd% %ServerCmd% %CenterConfig%
::start "DBServer-"%ServerCmd% %ServerCmd% %DBConfig%
::start "SceneCtrlServer-"%ServerCmd% %ServerCmd% %SceneCtrlConfig%
::start "SceneServer-"%ServerCmd% %ServerCmd% %SceneConfig%
::start "SceneServer1-"%ServerCmd% %ServerCmd% %Scene1Config%
::start "LoginServer-"%ServerCmd% %ServerCmd% %LoginConfig%
::start "Gateway-"%ServerCmd% %ServerCmd% %GatewayConfig%

::EXIT