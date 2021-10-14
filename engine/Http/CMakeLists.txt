
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(false)

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS} ${LOG4CPP_ROOT} ${GAMELIB_ROOT})
link_directories(${LOG4CPP_ROOT} ${Boost_LIBRARY_DIRS} ${LOG4CPP_ROOT})

link_curl_dir()

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(Http ${CURRENT_HEADERS} ${CURRENT_SRC} ${COMMON_SRC})
#add_executable(HttpServer ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(Http log4cpp)
if(WIN32)
	target_link_libraries(Http debug libcurl_debug optimized libcurl)
else()
	target_link_libraries(Http libcurl)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
