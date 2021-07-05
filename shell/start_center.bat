@echo off

call config.bat
::set ConfigFile="../../conf/center.cfg"

::cd ../bin/Debug

%ServerCmd% %CenterConfig%
::pause
