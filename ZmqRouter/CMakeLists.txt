
set(CMAKE_DEBUG_POSTFIX "_d") 

gnu_setting(true)

include_directories(${PROJECT_SOURCE_DIR}/log4cpp/include ${ZMQ_ROOT}/include ${GAMEUTIL_ROOT})
link_directories(${LOG4CPP_ROOT} ${ZMQ_ROOT}/libs ${PYTHON_ROOT}/libs)
file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_executable(ZmqRouter ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(ZmqRouter log4cpp GameUtil)
if(WIN32)
	target_link_libraries(ZmqRouter libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(ZmqRouter zmq)
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(ZmqRouter PROPERTIES DEBUG_POSTFIX "_d")
