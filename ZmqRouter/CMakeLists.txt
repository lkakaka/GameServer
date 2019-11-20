
set(CMAKE_DEBUG_POSTFIX "_d") 

include_directories(${PROJECT_SOURCE_DIR}/log4cpp/include ${ZMQ_ROOT}/include)
link_directories(${LOG4CPP_ROOT} ${ZMQ_ROOT}/libs)
file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_executable(ZmqRouter ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(ZmqRouter log4cpp libzmq-v142-mt-4_3_3)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
