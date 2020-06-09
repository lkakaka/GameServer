
set(CMAKE_DEBUG_POSTFIX "_d") 
#aux_source_directory(. DIR_LIB_SRCS)
file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(false)

include_directories(../log4cpp/include ../Depends/mysql/include include ${PYTHON_ROOT}/include ${REDIS_ROOT}/include ${GAMEUTIL_ROOT} ${ZMQ_ROOT}/include)
link_directories(../Depends/mysql/lib64 ${PYTHON_ROOT}/libs ${REDIS_ROOT}/libs ${GAMEUTIL_ROOT} ${ZMQ_ROOT}/libs)
link_libraries(log4cpp mysqlcppconn hiredis GameUtil)
add_library(DBPlugin SHARED ${CURRENT_HEADERS} ${CURRENT_SRC} ${COMMON_SRC})
if(WIN32)
	target_link_libraries(DBPlugin libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(DBPlugin zmq)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
