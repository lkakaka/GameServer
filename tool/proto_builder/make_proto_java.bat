@echo off

set PROTO_DIR=..\proto
set OUTPUT_DIR=..\client\GameClient\src\main\java

cd ..\proto

for /R %cd% %%f in (*.proto) do (
	..\tool\protoc --java_out=%OUTPUT_DIR% --proto_path=%PROTO_DIR% %%~nxf
	echo %%~nxf
	echo build proto file %%f
)

echo build proto finished

pause
