
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(false)

include_directories(${LOG4CPP_ROOT}/include ${PYTHON_ROOT}/include ${Boost_INCLUDE_DIRS} ${ZMQ_ROOT}/include)
link_directories(${LOG4CPP_ROOT} ${PYTHON_ROOT}/libs ${Boost_LIBRARY_DIRS} ${ZMQ_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(GameUtil SHARED ${CURRENT_HEADERS} ${CURRENT_SRC} ${COMMON_SRC})
target_link_libraries(GameUtil log4cpp)
if(WIN32)
	target_link_libraries(GameUtil libzmq-v142-mt-4_3_3)
else()
	target_link_libraries(GameUtil zmq python3.8)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
