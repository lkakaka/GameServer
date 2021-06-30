
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(true)

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS})
link_directories(${LOG4CPP_ROOT} ${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
#add_executable(Network ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC})
#add_library(Network ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC})
add_library(Network ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(Network log4cpp)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
set_target_properties(Network PROPERTIES DEBUG_POSTFIX "_d")
