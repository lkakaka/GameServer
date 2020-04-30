#!/bin/bash
echo "call set_path.sh"

DEPENDS_ROOT=../Depends
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:$DEPENDS_ROOT/Python38/libs:$DEPENDS_ROOT/mysql/lib64:$DEPENDS_ROOT/zmq/libs

ulimit -c unlimited
#echo "coredump-%e-%t" > /proc/sys/kernel/core_pattern
