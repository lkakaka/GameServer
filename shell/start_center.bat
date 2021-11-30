@echo off

call start.bat center all

::call config.bat
::set ConfigFile="../../conf/101/center.cfg"

::cd ../bin/Debug

::%ServerCmd% %CenterConfig%
::pause
