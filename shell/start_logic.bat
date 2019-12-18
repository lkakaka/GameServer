@echo off

set ServerCmd="GameServer_d.exe"
set ConfigFile="../../conf/scene.cfg"

cd ../bin/Debug
:: md %BUILD_PATH%

%ServerCmd% %ConfigFile%
pause
