#!/bin/bash
while read choice;do
echo "你输入的是$choice"
cd ..
if [ "$choice" == "restart" ]; then
./stop.sh all
sleep 3
./start.sh all
elif [ "$choice" == "start_all" ]; then
./start.sh all
elif [ $choice == "stop_all" ]; then
./stop.sh all
fi
cd wsd
done
