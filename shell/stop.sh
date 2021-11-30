#!/bin/bash
basedir=`dirname $0`
#echo $basedir
cd $basedir

. ./check.sh

stopAll(){
    for file in pid/$1/*
	do
	#echo $file
	pid=`sed -n '1 p' $file`
	echo 'stop' $pid
	kill -15 $pid
	done
}

stopServer(){
	pid_file=pid/$1/$2.pid
	if [ ! -f "$pid_file" ]; then
		echo pid file $pid_file not exist!!!
		return
	fi
	pid=`sed -n '1 p' $pid_file`
	echo 'stop' $pid
	kill -15 $pid
}

usage(){
	echo "Usage: stop [server_id] [all|login|center...]"
}

case $2 in
all)
	stopAll $1
    sleep 1
    result=`check_all_stop $1 "stop failed"`
    if [ -z "$result" ];then
        echo "all server stop successful"
    fi
	;;
*)
	if [ -f pid/$1/$2.pid ];then
		stopServer $1 $2	
        sleep 1
        result=`check_server_stop $1 $2 "stop failed"`
        if [ -z "$result" ];then
            echo "$2 stop successful"
        fi
	else
		usage
	fi
	;;	
esac
