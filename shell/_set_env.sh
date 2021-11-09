#!/bin/bash

basedir=`cd $(dirname $0);pwd -P`
bindir=$basedir/../bin
logdir=$basedir/../log

DEPENDS_ROOT=$basedir/../engine/Depends
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:$DEPENDS_ROOT/Python38/libs:$DEPENDS_ROOT/mysql/lib64:$DEPENDS_ROOT/zmq/libs:$DEPENDS_ROOT/mongo-c-driver/libs

VERSION_RELEASE=Release
VERSION_DEBUG=Debug

#select debug or releas version
VERSION=$VERSION_DEBUG
#VERSION=$VERSION_RELEASE

if [ "$VERSION" == "$VERSION_RELEASE" ];then
GAME_SERVER=$bindir/GameServer
GAME_TEST=$bindir/Test
else
#export ASAN_OPTIONS=halt_on_error=0:use_sigaltstack=0:detect_leaks=1:malloc_context_size=15:log_path=$logdir/asan.log
GAME_SERVER=$bindir/GameServer_d
GAME_TEST=$bindir/Test
fi

ulimit -c unlimited
#echo "coredump-%e-%t" > /proc/sys/kernel/core_pattern
