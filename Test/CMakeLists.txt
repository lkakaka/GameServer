
set(CMAKE_DEBUG_POSTFIX "_d") 

gnu_setting(false)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_executable(Test ${CURRENT_HEADERS} ${CURRENT_SRC})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
