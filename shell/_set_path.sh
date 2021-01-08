#!/bin/bash
echo "call set_path.sh"

DEPENDS_ROOT=../engine/Depends
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:$DEPENDS_ROOT/Python38/libs:$DEPENDS_ROOT/mysql/lib64:$DEPENDS_ROOT/zmq/libs:$DEPENDS_ROOT/mongo-c-driver/libs

VERSION_RELEASE=Release
VERSION_DEBUG=Debug

#select debug or releas version
VERSION=$VERSION_DEBUG
#VERSION=$VERSION_RELEASE

if [ "$VERSION" == "$VERSION_RELEASE" ];then
GAME_SERVER=../engine/bin/GameServer
else
GAME_SERVER=../engine/bin/GameServer_d
fi

ulimit -c unlimited
#echo "coredump-%e-%t" > /proc/sys/kernel/core_pattern
