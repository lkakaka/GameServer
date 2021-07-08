#!/bin/bash
redis-server 6379/redis.conf &
redis-server 6380/redis.conf &
redis-server 6381/redis.conf &
redis-server 6382/redis.conf &
redis-server 6383/redis.conf &
redis-server 6384/redis.conf &
echo "sleep start-----------"
sleep 3
echo "sleep end-----------"
redis-cli -c --cluster create 127.0.0.1:6379 127.0.0.1:6380 127.0.0.1:6381 127.0.0.1:6382 127.0.0.1:6383 127.0.0.1:6384 --cluster-replicas 1
