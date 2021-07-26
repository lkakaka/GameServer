
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(false)
gnu_cxx_flag("-fPIC")

include_directories(${SOL_ROOT} ${LOG4CPP_ROOT}/include ${PYTHON_ROOT}/include ${GAMELIB_ROOT} ${Boost_INCLUDE_DIRS} ${SCENE_ROOT} ${PROTO_ROOT} ${PROTOBUFF_ROOT}/include
${DBPLUGIN_ROOT} ${DBPLUGIN_ROOT}/include ${REDIS_ROOT}/include ${MYSQL_ROOT}/include ${HTTP_SERVER_ROOT})
link_directories(${LOG4CPP_ROOT} ${PYTHON_ROOT}/libs ${GAMELIB_ROOT} ${Boost_LIBRARY_DIRS} ${SCENE_ROOT} ${DBPLUGIN_ROOT} ${HTTP_SERVER_ROOT})

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *.cc *.c)

set(all_files ${CURRENT_HEADERS} ${CURRENT_SRC})
source_group_by_dir(all_files)

add_library(PythonBind ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(PythonBind log4cpp GameLib SceneServer DBPlugin HttpServer)
if (NOT WIN32)
target_link_libraries(PythonBind python3.8)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
