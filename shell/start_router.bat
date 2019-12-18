@echo off

set ServerCmd="ZmqRouter_d.exe"
set ConfigFile="../../conf/router.cfg"

cd ../bin/Debug

%ServerCmd% %ConfigFile%
pause
