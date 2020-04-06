
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

if (CMAKE_COMPILER_IS_GNUCXX)
 	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
 	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS})
link_directories(${LOG4CPP_ROOT} ${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(HttpServer SHARED ${CURRENT_HEADERS} ${CURRENT_SRC})
#add_executable(HttpServer ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(HttpServer log4cpp)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
