@echo off

call config.bat

::cd ../bin/Debug
:: md %BUILD_PATH%

%ServerCmd% %DBConfig%
::pause
