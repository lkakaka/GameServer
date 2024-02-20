#!/bin/sh

SHORT=s:b:,h
LONG=script:,build_type:,help
OPTS=$(getopt -a -n _cmake --options $SHORT --longoptions $LONG -- "$@")
#echo $OPTS

help()
{
	USAGE="Usage: _cmake.sh [option] [arg]\n
	options:\n
	[-s|--script]           PYTHON or LUA, default option is LUA\n
	[-b|--build_type]       Release or Debug, default option is Debug\n
	[-h|--help]             print help"	
	echo $USAGE
	exit 2	
}

# default is lua
SCRIPT="LUA"
BUILD_TYPE="Debug"
eval set -- "$OPTS"

while true ;
do
	case "$1" in
		-s|--script)
			SCRIPT="$2"
			shift 2
			;;
		-b|--build_type)
			BUILD_TYPE="$2"
			shift 2
			;;
		-h | --help)
			help
			;;
		--)
			shift
			break
			;;
		*)
			echo "unkown option: $1"
			help
			;;
	esac
done

echo "script: $SCRIPT"
echo "build type: $BUILD_TYPE"
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DLOGIC_SCRIPT=$SCRIPT -S ./ -B ./build

