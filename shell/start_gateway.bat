@echo off

set ServerCmd="Gateway_d.exe"
set ConfigFile="../../conf/gateway.cfg"

cd ../bin/Debug

%ServerCmd% %ConfigFile%
pause
