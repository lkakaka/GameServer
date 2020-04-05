
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

if (CMAKE_COMPILER_IS_GNUCXX)
 	set(CMAKE_CXX_FLAGS "-std=c++11 -pthread ${CMAKE_CXX_FLAGS}")
 	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS} ${PROTOBUFF_ROOT}/src ${GAMEUTIL_ROOT} ${ZMQ_ROOT}/include ${PROTO_ROOT})
link_directories(${LOG4CPP_ROOT} ${Boost_LIBRARY_DIRS} ${PROTOBUFF_ROOT}/cmake/build ${GAMEUTIL_ROOT} ${ZMQ_ROOT}/libs ${PYTHON_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_executable(Gateway ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC})
target_link_libraries(Gateway log4cpp GameUtil)
if(WIN32)
	target_link_libraries(Gateway debug libprotobufd optimized libprotobuf)
	target_link_libraries(Gateway libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(Gateway protobuf)
	target_link_libraries(Gateway zmq)
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(Gateway PROPERTIES DEBUG_POSTFIX "_d")
