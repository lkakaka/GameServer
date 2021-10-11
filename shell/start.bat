@echo off

call config.bat

setlocal enabledelayedexpansion
echo %1
IF "%1" == "all" (
	call:startAll
) ELSE (
	call:startServer %1
)

exit /b 0

:startAll
	::echo "startAll"
	for %%i in (conf\*.cfg) do ( 
		::echo %%i
		set cfg_file=%%i
		::echo !cfg_file!
		set server_name=!cfg_file:~5,-4!
		::echo !server_name!
		call:startServer !server_name!
	)
GOTO:EOF

:startServer
	set cfg_file="conf/%1.cfg"
	::echo %cfg_file%
	if exist %cfg_file% (
		echo start server %1
		start "%1-"%ServerCmd% %ServerCmd% %cfg_file%
	) else (
		echo usage: start [all^|center^|login...]
	)
GOTO:EOF

::pause
