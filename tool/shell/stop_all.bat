@ECHO OFF

call config.bat

taskkill /f /t /im %GatewayExe%
taskkill /f /t /im %GameServerExe%
taskkill /f /t /im %RouterExe%

::EXIT