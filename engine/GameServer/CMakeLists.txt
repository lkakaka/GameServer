
# set(PROTOBUFF_ROOT ../../protobuf-3.9.0)
# set(BOOST_ROOT ../../boost_1_70_0)
# set(PYTHON_LIBRARY C:/Python34/libs)
# set(DBPLUGIN_ROOT ${PROJECT_SOURCE_DIR}/DBPlugin)
# find_package(Boost REQUIRED)
# find_package(PythonLibs REQUIRED)

add_definitions(-DOPEN_PROFILE_TRACK)
if (WIN32)
	add_definitions(-DLUA_BUILD_AS_DLL)
endif (WIN32)

gnu_setting(false)

link_directories(${Boost_LIBRARY_DIRS} ${PYTHON_ROOT}/libs ${LOG4CPP_ROOT} ${PROTOBUFF_ROOT}/libs 
${DEPENDS_ROOT}/mysql/lib64 ${DBPLUGIN_ROOT} ${ZMQ_ROOT}/libs ${GAMEUTIL_ROOT} ${SCENE_ROOT} ${HTTP_SERVER_ROOT} ${GATEWAY_ROOT} ${REDIS_ROOT}/libs ${MONGO_ROOT}/libs 
${LUA_ROOT}/libs)
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_ROOT}/include ${LOG4CPP_ROOT}/include ${PROTOBUFF_ROOT}/include 
${DBPLUGIN_ROOT} ${DBPLUGIN_ROOT}/include ${ZMQ_ROOT}/include ${GAMEUTIL_ROOT} ${PROTO_ROOT} ${SCENE_ROOT} ${HTTP_SERVER_ROOT} ${DEPENDS_ROOT}/mysql/include
${GATEWAY_ROOT} ${REDIS_ROOT}/include ${MONGO_ROOT}/include ${LUA_ROOT}/include)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")
	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (NOT WIN32)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *.cc)

file(GLOB_RECURSE PY_SRC  ../script/python/*.py)

set(all_files ${CURRENT_HEADERS} ${CURRENT_SRC} ${PY_SRC})
source_group_by_dir(all_files)

#SOURCE_GROUP("PyScript" FILES ${PY_SRC})

# 使用aux_source_directory vs中头文件会无法显示
# aux_source_directory(./ MYSERVER_SRC_DIR)
# aux_source_directory(./proto MYSERVER_PROTO_SRC_DIR)
# aux_source_directory(./db MYSERVER_DB_DIR)
# aux_source_directory(./py_module MYSERVER_PY_MOD_DIR)
# add_executable(GameServer ${MYSERVER_SRC_DIR} ${MYSERVER_PROTO_SRC_DIR} ${MYSERVER_DB_DIR} ${MYSERVER_PY_MOD_DIR})
add_executable(GameServer ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC} ${COMMON_SRC} ${PY_SRC})

# message(STATUS ${CURRENT_HEADERS})
# SOURCE_GROUP("include" FILES ${CURRENT_HEADERS})
add_dependencies(GameServer DBPlugin)
target_link_libraries(GameServer MathFunction log4cpp DBPlugin GameUtil SceneServer HttpServer Gateway hiredis)
if(WIN32)
	target_link_libraries(GameServer debug libprotobufd optimized libprotobuf)
	target_link_libraries(GameServer libzmq-v142-mt-4_3_3 lua)
else()
	target_link_libraries(GameServer protobuf)
	target_link_libraries(GameServer zmq liblua.a dl)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_OUT_PATH})
set_target_properties(GameServer PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(GameServer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_OUT_PATH}")

