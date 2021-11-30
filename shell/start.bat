@echo off

call config.bat

setlocal enabledelayedexpansion
echo %1 %2
IF "%2" == "all" (
	call:startAll %1
) ELSE (
	call:startServer %1 %2
)

exit /b 0

:startAll
	::echo "startAll" %1
	for %%i in (conf\%1\*.cfg) do ( 
		::echo %%i
		set cfg_file=""
		::echo "start for"
		FOR /f "tokens=3* delims=\" %%s IN ("%%i") DO (
			set cfg_file=%%s
		)
		::echo "end for"
		::set cfg_file=%%i
		::echo !cfg_file!
		set server_name=!cfg_file:~0,-4!
		::echo !server_name!
		call:startServer %1 !server_name!
	)
GOTO:EOF

:startServer
	set cfg_file="conf/%1/%2.cfg"
	::echo %cfg_file%
	if exist %cfg_file% (
		echo start server %1
		start "%1:%2-"%ServerCmd% %ServerCmd% %cfg_file%
	) else (
		echo usage: start [server] [all^|center^|login...]
	)
GOTO:EOF

::pause
