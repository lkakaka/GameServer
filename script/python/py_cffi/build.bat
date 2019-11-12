@echo off

set python="C:/Python38/python"

:: md %BUILD_PATH%
%python% cffi_builder.py

pause
