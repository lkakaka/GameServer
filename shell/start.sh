#!/bin/bash
basedir=`dirname $0`
#echo $basedir
cd $basedir

. ./check.sh

setEnv(){
	source _set_env.sh
	#echo $LD_LIBRARY_PATH
}

create_dir()
{   
	if [ ! -d "$1" ];then
		mkdir -p $1
	fi
}

startAll(){
    result=`check_all_stop $1 "not stopped"`
    if [ -n "$result" ];then
        echo $result
        exit
    fi

	for cfg_file in conf/$1/*
	do
	#echo $cfg_file
	server_name=${cfg_file%.*}
	server_name=${server_name##*/}
	#echo $server_name
	startServer $1 $server_name
	done
}

startServer(){
    result=`check_server_stop $1 $2 "not stopped"`
    if [ -n "$result" ];then
        echo $result
        exit
    fi
	
	pid_dir=pid/$1
    create_dir $pid_dir
    log_dir=../log/$1
    create_dir $log_dir
    
	$GAME_SERVER conf/$1/$2.cfg >> $log_dir/$2_output.log 2>&1 &
	echo $! > $pid_dir/$2.pid
	echo "start" $2
}

startTest() {
	$GAME_TEST
	#echo $! > pid/test.pid
	echo "start test"
}

usage(){
	echo "Usage: start [server_id] [all|center|login...]"
}

case $2 in
all)
	setEnv
	startAll $1
    sleep 1
    result=`check_all_start $1 "start failed"`
    if [ -z "$result" ];then
        echo "all server start successful"
    fi
	;;
test)
	setEnv
	startTest
	;;
*)
	if [ -f "conf/$1/$2.cfg" ];then
		setEnv
		startServer $1 $2
        sleep 1
        result=`check_server_start $1 $2 "start failed"`
        if [ -z "$result" ];then
            echo "$2 start successful"
        fi
	else
		usage	
	fi
	;;
esac

