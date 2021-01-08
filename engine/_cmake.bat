@echo off

set BUILD_PATH="./build"

:: md %BUILD_PATH%

cmake -DRECASTNAVIGATION_STATIC=1 -S ./ -B %BUILD_PATH%

pause
