
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(true)

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS} ${PROTOBUFF_ROOT}/include ${GAMELIB_ROOT} ${ZMQ_ROOT}/include ${PROTO_ROOT})
link_directories(${LOG4CPP_ROOT} ${PROTOBUFF_ROOT}/libs ${GAMELIB_ROOT} ${ZMQ_ROOT}/libs ${PYTHON_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
#add_executable(Gateway ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC})
#add_library(Gateway ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC})
add_library(Gateway ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(Gateway log4cpp GameLib ProtoBuffer)
if(WIN32)
	target_link_libraries(Gateway debug libprotobufd optimized libprotobuf)
	target_link_libraries(Gateway libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(Gateway protobuf)
	target_link_libraries(Gateway zmq)
endif()
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
set_target_properties(Gateway PROPERTIES DEBUG_POSTFIX "_d")
