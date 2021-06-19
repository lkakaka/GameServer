@echo off

call config.bat
::set ConfigFile="../../conf/scene_ctrl.cfg"

::cd ../bin/Debug
:: md %BUILD_PATH%

%ServerCmd% %SceneCtrlConfig%
::pause
