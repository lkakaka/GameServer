#!/bin/bash

check_all_start()
{
    for file in pid/*
    do
    name=`echo $file | awk -F'[/.]' '{print $2}'`
    check_server_start $name "$1"
    done
}

check_all_stop()
{
    for file in pid/*
    do
    name=`echo $file | awk -F'[/.]' '{print $2}'`
    check_server_stop $name "$1"
    done
}

check_server_start()
{
     pid=`sed -n '1 p' pid/$1.pid`
     pcount=`ps -ef | grep $pid | grep -v 'grep' | wc -l`
     if [ $pcount == 0 ];then
        echo "$1 $2!!!"
     fi
}

check_server_stop()
{
     pid=`sed -n '1 p' pid/$1.pid`
     pcount=`ps -ef | grep $pid | grep -v 'grep' | wc -l`
     if [ $pcount == 1 ];then
        echo "$1 $2!!!"
     fi
}

#check_all_stop
