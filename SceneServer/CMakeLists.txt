
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSCENE_EXPORT -DOPEN_PROFILE_TRACK)

include_directories(${LOG4CPP_ROOT}/include ${PYTHON_ROOT}/include ${Boost_INCLUDE_DIRS})
link_directories(${LOG4CPP_ROOT} ${PYTHON_ROOT}/libs ${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(SceneServer SHARED ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(SceneServer log4cpp)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
