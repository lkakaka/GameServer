@ECHO OFF

call config.bat

taskkill /f /t /im %GameServerExe%

::EXIT