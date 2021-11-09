#!/bin/bash
basedir=`dirname $0`
#echo $basedir
cd $basedir

. ./check.sh

stopAll(){
    for file in pid/*
	do
	#echo $file
	pid=`sed -n '1 p' $file`
	echo 'stop' $pid
	kill -15 $pid
	done
}

stopServer(){
	pid=`sed -n '1 p' pid/$1.pid`
	echo 'stop' $pid
	kill -15 $pid
}

usage(){
	echo "Usage: stop [all|login|center...]"
}

case $1 in
all)
	stopAll
    sleep 1
    result=`check_all_stop "stop failed"`
    if [ -z "$result" ];then
        echo "all server stop successful"
    fi
	;;
*)
	if [ -f pid/$1.pid ];then
		stopServer $1	
        sleep 1
        result=`check_server_stop "stop failed"`
        if [ -z "$result" ];then
            echo "$1 stop successful"
        fi
	else
		usage
	fi
	;;	
esac
