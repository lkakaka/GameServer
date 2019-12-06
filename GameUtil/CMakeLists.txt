
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DGAMEUTIL_EXPORT)

include_directories(${LOG4CPP_ROOT}/include ${PYTHON_ROOT}/include ${Boost_INCLUDE_DIRS})
link_directories(${LOG4CPP_ROOT} ${PYTHON_ROOT}/libs ${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(GameUtil SHARED ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(GameUtil log4cpp)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
