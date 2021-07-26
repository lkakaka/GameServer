
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(false)
gnu_cxx_flag("-fPIC")

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS} ${ZMQ_ROOT}/include)
link_directories(${LOG4CPP_ROOT} ${Boost_LIBRARY_DIRS} ${ZMQ_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *.cc *.c)

set(all_files ${CURRENT_HEADERS} ${CURRENT_SRC})
source_group_by_dir(all_files)

add_library(GameLib ${CURRENT_HEADERS} ${CURRENT_SRC} ${COMMON_SRC})
target_link_libraries(GameLib log4cpp)
if(WIN32)
	target_link_libraries(GameLib libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(GameLib zmq)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
