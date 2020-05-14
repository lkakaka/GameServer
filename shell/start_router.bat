@echo off

call config.bat
::set ConfigFile="../../conf/router.cfg"

::cd ../bin/Debug

%ServerCmd% %RouterConfig%
::pause
