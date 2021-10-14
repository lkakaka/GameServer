
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)
add_definitions(-DLUA_BUILD_AS_DLL)

gnu_setting(false)
gnu_cxx_flag("-fPIC")
open_vs_cplus17() # sol¿âÐèÒªc++17

include_directories(${SOL_ROOT} ${LOG4CPP_ROOT}/include ${LUA_ROOT}/include ${GAMELIB_ROOT} ${Boost_INCLUDE_DIRS} ${SCENE_ROOT} ${PROTO_ROOT} ${PROTOBUFF_ROOT}/include
${DBPLUGIN_ROOT} ${DBPLUGIN_ROOT}/include ${REDIS_ROOT}/include ${MYSQL_ROOT}/include)
link_directories(${LOG4CPP_ROOT} ${LUA_ROOT}/libs ${GAMELIB_ROOT} ${Boost_LIBRARY_DIRS} ${SCENE_ROOT} ${DBPLUGIN_ROOT})

link_curl_dir()
link_http_dir()

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *.cc *.c)

set(all_files ${CURRENT_HEADERS} ${CURRENT_SRC})
source_group_by_dir(all_files)

add_library(LuaBind ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(LuaBind log4cpp GameLib SceneServer DBPlugin liblua Http)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
