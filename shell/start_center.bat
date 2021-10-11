@echo off

call start.bat center

::call config.bat
::set ConfigFile="../../conf/center.cfg"

::cd ../bin/Debug

::%ServerCmd% %CenterConfig%
::pause
