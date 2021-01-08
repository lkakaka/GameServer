#!/bin/sh

if [ "$1" = "release" ]; then
	echo "release version"	
	cmake -S ./ -B ./build
else
	echo "debug version "
	cmake -DCMAKE_BUILD_TYPE=Debug -S ./ -B ./build
fi
