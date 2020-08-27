#!/bin/bash
source _set_path.sh
echo $LD_LIBRARY_PATH
if [ "$VERSION" == "$VERSION_RELEASE" ];then
../bin/GameServer ../conf/scene_ctrl.cfg &
else
../bin/GameServer_d ../conf/scene_ctrl.cfg &
fi
