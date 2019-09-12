@echo off

set BUILD_PATH="./build"

:: md %BUILD_PATH%

cmake -S ./ -B %BUILD_PATH%

pause
