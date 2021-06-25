
set(CMAKE_DEBUG_POSTFIX "_d") 

gnu_setting(false)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_executable(Test ${CURRENT_HEADERS} ${CURRENT_SRC})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_OUT_PATH})
