#!/bin/bash
source _set_path.sh
echo $LD_LIBRARY_PATH
../bin/Gateway_d ../conf/gateway.cfg &
