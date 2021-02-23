#!/bin/bash
while read choice;do
echo "你输入的是$choice"
cd ..
if [ "$choice" == "restart" ]; then
./stop_all.sh
./start_all.sh
elif [ "$choice" == "start_all" ]; then
./start_all.sh
elif [ $choice == "stop_all" ]; then
./stop_all.sh
fi
cd wsd
done
