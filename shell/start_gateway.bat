@echo off

call config.bat
::set ConfigFile="../../conf/gateway.cfg"

::cd ../bin/Debug

%ServerCmd% %GatewayConfig%
::pause
