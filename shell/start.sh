#!/bin/bash
basedir=`dirname $0`
#echo $basedir
cd $basedir

setEnv(){
	source _set_env.sh
	echo $LD_LIBRARY_PATH
}

startAll(){
	for cfg_file in conf/*
	do
	#echo $cfg_file
	server_name=${cfg_file%.*}
	server_name=${server_name##*/}
	#echo $server_name
	startServer $server_name
	done
}

startServer(){
	$GAME_SERVER conf/$1.cfg &
	echo $! > pid/$1.pid
	echo "start" $1
}

startTest() {
	$GAME_TEST
	#echo $! > pid/test.pid
	echo "start test"
}

usage(){
	echo "Usage: start [all|center|login...]"
}

case $1 in
all)
	setEnv
	startAll
	;;
test)
	setEnv
	startTest
	;;
*)
	if [ -f "conf/$1.cfg" ];then
		setEnv
		startServer $1
	else
		usage	
	fi
	;;
esac

