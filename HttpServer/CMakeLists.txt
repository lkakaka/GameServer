
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DHTTP_SERVER_EXPORT -DOPEN_PROFILE_TRACK)

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS})
link_directories(${LOG4CPP_ROOT} ${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(HttpServer SHARED ${CURRENT_HEADERS} ${CURRENT_SRC})
#add_executable(HttpServer ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(HttpServer log4cpp)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
