@echo off

call config.bat
set ConfigFile="../../conf/router.cfg"

cd ../bin/Debug

%RouterCmd% %ConfigFile%
::pause
