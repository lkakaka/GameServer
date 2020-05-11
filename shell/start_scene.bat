@echo off

call config.bat
::set ConfigFile="../../conf/scene.cfg"

::cd ../bin/Debug
:: md %BUILD_PATH%

%ServerCmd% %SceneConfig%
::pause
