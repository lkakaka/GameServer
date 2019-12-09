
set(PROTOBUFF_ROOT ../../protobuf-3.9.0)
# set(BOOST_ROOT ../../boost_1_70_0)
# set(PYTHON_LIBRARY C:/Python34/libs)
set(DBPLUGIN_ROOT ${PROJECT_SOURCE_DIR}/DBPlugin)
# find_package(Boost REQUIRED)
# find_package(PythonLibs REQUIRED)

add_definitions(-DOPEN_PROFILE_TRACK)

link_directories(${Boost_LIBRARY_DIRS} ${PYTHON_ROOT}/libs ${LOG4CPP_ROOT} ${PROTOBUFF_ROOT}/cmake/build ${DEPENDS_ROOT}/mysql/lib64/vs14 ${DBPLUGIN_ROOT} ${ZMQ_ROOT}/libs ${GAMEUTIL_ROOT})
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_ROOT}/include ${LOG4CPP_ROOT}/include ${PROTOBUFF_ROOT}/src ${DBPLUGIN_ROOT} ${DBPLUGIN_ROOT}/include ${ZMQ_ROOT}/include ${GAMEUTIL_ROOT})

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")
	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (NOT WIN32)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)

# 使用aux_source_directory vs中头文件会无法显示
# aux_source_directory(./ MYSERVER_SRC_DIR)
# aux_source_directory(./proto MYSERVER_PROTO_SRC_DIR)
# aux_source_directory(./db MYSERVER_DB_DIR)
# aux_source_directory(./py_module MYSERVER_PY_MOD_DIR)
# add_executable(GameServer ${MYSERVER_SRC_DIR} ${MYSERVER_PROTO_SRC_DIR} ${MYSERVER_DB_DIR} ${MYSERVER_PY_MOD_DIR})
add_executable(GameServer ${CURRENT_HEADERS} ${CURRENT_SRC})

# message(STATUS ${CURRENT_HEADERS})
# SOURCE_GROUP("include" FILES ${CURRENT_HEADERS})

target_link_libraries(GameServer MathFunction log4cpp DBPlugin GameUtil libzmq-v142-mt-4_3_3)
target_link_libraries(GameServer debug libprotobufd optimized libprotobuf)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(GameServer PROPERTIES DEBUG_POSTFIX "_d")

