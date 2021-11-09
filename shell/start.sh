#!/bin/bash
basedir=`dirname $0`
#echo $basedir
cd $basedir

. ./check.sh

setEnv(){
	source _set_env.sh
	#echo $LD_LIBRARY_PATH
}

startAll(){
    result=`check_all_stop "not stopped"`
    if [ -n "$result" ];then
        echo $result
        exit
    fi

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
    result=`check_server_stop $1 "not stopped"`
    if [ -n "$result" ];then
        echo $result
        exit
    fi
    
	$GAME_SERVER conf/$1.cfg >> ../log/$1_output.log 2>&1 &
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
    sleep 1
    result=`check_all_start "start failed"`
    if [ -z "$result" ];then
        echo "all server start successful"
    fi
	;;
test)
	setEnv
	startTest
	;;
*)
	if [ -f "conf/$1.cfg" ];then
		setEnv
		startServer $1
        sleep 1
        result=`check_server_start $1 "start failed"`
        if [ -z "$result" ];then
            echo "$1 start successful"
        fi
	else
		usage	
	fi
	;;
esac

