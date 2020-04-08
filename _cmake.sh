#!/bin/sh

if [ "$1" = "debug" ]; then
	echo "debug version "
	cmake -DCMAKE_BUILD_TYPE=Debug -S ./ -B ./build
else
	echo "release version"	
	cmake -S ./ -B ./build
fi
