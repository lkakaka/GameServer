#!/bin/bash

ps -ux --no-headers |grep GameServer |grep -v 'grep' |awk '{if($2>0) {print "kill gameServer "$2" "$12; cmd="kill -15 "$2; system(cmd)}}'
#ps -ux --no-headers |grep Gateway |grep -v 'grep' |awk '{if($2>0) {print "kill gateway "$2" "$12; cmd="kill -9 "$2; system(cmd)}}'
#ps -ux --no-headers |grep ZmqRouter |grep -v 'grep' |awk '{if($2>0) {print "kill zmqRouter "$2" "$12; cmd="kill -9 "$2; system(cmd)}}'
#if [ $pid gt 0 ];then
#kill -9 $pid
#fi
echo "finished"
