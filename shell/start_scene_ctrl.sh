#!/bin/bash
source _set_path.sh
echo $LD_LIBRARY_PATH
$GAME_SERVER ./conf/scene_ctrl.cfg &
