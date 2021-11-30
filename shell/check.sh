#!/bin/bash

check_all_start()
{
    if [ ! -d "pid/$1" ];then
        return
    fi

    for file in pid/$1/*
    do
    name=`echo $file | awk -F'[/.]' '{print $3}'`
    check_server_start $name "$2"
    done
}

check_all_stop()
{
    if [ ! -d "pid/$1" ];then
        return
    fi

    for file in pid/$1/*
    do
    name=`echo $file | awk -F'[/.]' '{print $3}'`
    check_server_stop $1 $name "$2"
    done
}

check_server_start()
{
     if [ ! -f "pid/$1/$2.pid" ];then
        return
     fi

     pid=`sed -n '1 p' pid/$1/$2.pid`
     pcount=`ps -ef | grep $pid | grep -v 'grep' | wc -l`
     if [ $pcount == 0 ];then
        echo "$2 $3!!!"
     fi
}

check_server_stop()
{
     if [ ! -f "pid/$1/$2.pid" ];then
        return
     fi

     pid=`sed -n '1 p' pid/$1/$2.pid`
     pcount=`ps -ef | grep $pid | grep -v 'grep' | wc -l`
     if [ $pcount == 1 ];then
        echo "$2 $3!!!"
     fi
}

#check_all_stop
