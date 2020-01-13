@echo off

call config.bat
set ConfigFile="../../conf/db.cfg"

cd ../bin/Debug
:: md %BUILD_PATH%

%ServerCmd% %ConfigFile%
::pause
