#!/bin/bash
basedir=`dirname $0`
#echo $basedir
cd $basedir

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
	;;
*)
	if [ -f pid/$1.pid ];then
		stopServer $1	
	else
		usage
	fi
	;;	
esac
