#!/bin/bash
source _set_path.sh
echo $LD_LIBRARY_PATH
../bin/ZmqRouter ../conf/router.cfg &
