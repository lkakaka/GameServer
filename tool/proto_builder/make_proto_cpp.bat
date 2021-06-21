@echo off

set PROTO_DIR=..\proto
set OUTPUT_DIR=..\MyServer\proto

cd ..\proto

for /R %cd% %%f in (*.proto) do (
	..\tool\protoc --cpp_out=%OUTPUT_DIR% --proto_path=%PROTO_DIR% %%~nxf
	echo %%~nxf
	echo build proto file %%f
)

echo build proto finished

pause
