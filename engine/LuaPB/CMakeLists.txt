
#set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK -DLUA_BUILD_AS_DLL)

gnu_setting(false)
#gnu_cxx_flag("-fPIC")

include_directories(${LUA_ROOT}/include)
link_directories(${LUA_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *.cc *.c)

set(all_files ${CURRENT_HEADERS} ${CURRENT_SRC})
source_group_by_dir(all_files)

add_library(pb SHARED ${CURRENT_HEADERS} ${CURRENT_SRC})
if(WIN32)
	target_link_libraries(pb lua)
else()
	target_link_libraries(pb liblua.a dl)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
