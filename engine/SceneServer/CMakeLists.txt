
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)
#add_definitions(-DDT_POLYREF64) # polyref π”√64Œª

gnu_setting(false)

include_directories(${LOG4CPP_ROOT}/include ${PYTHON_ROOT}/include ${Boost_INCLUDE_DIRS} ${PROTOBUFF_ROOT}/include ${PROTO_ROOT} ${GAMELIB_ROOT} ${ZMQ_ROOT}/include)
link_directories(${LOG4CPP_ROOT} ${PYTHON_ROOT}/libs ${Boost_LIBRARY_DIRS} ${PROTOBUFF_ROOT}/libs ${GAMELIB_ROOT} ${ZMQ_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
#add_library(SceneServer SHARED ${CURRENT_HEADERS} ${CURRENT_SRC} ${COMMON_SRC} ${PROTO_SRC})
add_library(SceneServer SHARED ${CURRENT_HEADERS} ${CURRENT_SRC} ${COMMON_SRC})
target_link_libraries(SceneServer log4cpp GameLib Detour ProtoBuffer)
if(WIN32)
	target_link_libraries(SceneServer debug libprotobufd optimized libprotobuf)
	target_link_libraries(SceneServer libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(SceneServer protobuf)
	target_link_libraries(SceneServer zmq)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
