
set(CMAKE_DEBUG_POSTFIX "_d") 

gnu_setting(false)
open_vs_cplus17()

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)

link_directories(${Boost_LIBRARY_DIRS} ${LUA_ROOT}/libs)
include_directories(${Boost_INCLUDE_DIRS} ${SOL_ROOT} ${LUA_ROOT}/include)

add_executable(Test ${CURRENT_HEADERS} ${CURRENT_SRC})
if(WIN32)
	target_link_libraries(Test lua)
else()
	#target_link_libraries(Test liblua.a dl)
	target_link_libraries(Test liblua)
endif()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_OUT_PATH})
