@ECHO OFF

call config.bat

taskkill /f /t /im %GatewayCmd%
taskkill /f /t /im %ServerCmd%
taskkill /f /t /im %RouterCmd%

::EXIT