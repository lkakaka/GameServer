
set(CMAKE_DEBUG_POSTFIX "_d") 

gnu_setting(true)
open_vs_cplus17()

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)

link_directories(${Boost_LIBRARY_DIRS} ${LUA_ROOT}/libs ${LOG4CPP_ROOT})
include_directories(${Boost_INCLUDE_DIRS} ${SOL_ROOT} ${LUA_ROOT}/include ${LOG4CPP_ROOT}/include)

link_curl_dir()

add_executable(Test ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(Test liblua log4cpp)
if(WIN32)
	target_link_libraries(Test debug libcurl_debug optimized libcurl)
else()
	target_link_libraries(Test libcurl)
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_OUT_PATH})
