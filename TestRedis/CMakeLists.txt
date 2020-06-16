
set(CMAKE_DEBUG_POSTFIX "_d") 

gnu_setting(false)

include_directories(${LOG4CPP_ROOT}/include ${REDIS_ROOT}/include)
link_directories(${LOG4CPP_ROOT} ${REDIS_ROOT}/libs)
file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_executable(TestRedis ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(TestRedis log4cpp hiredis)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
