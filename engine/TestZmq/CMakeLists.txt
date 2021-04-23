
set(CMAKE_DEBUG_POSTFIX "_d") 

gnu_setting(true)

include_directories(${PROJECT_SOURCE_DIR}/log4cpp/include ${ZMQ_ROOT}/include)
link_directories(${LOG4CPP_ROOT} ${ZMQ_ROOT}/libs)
file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_executable(TestZmq ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(TestZmq log4cpp)
if(WIN32)
	target_link_libraries(TestZmq libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(TestZmq zmq)
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_OUT_PATH})
