cmake_minimum_required(VERSION 3.0)
project(GameServer)

add_subdirectory(Math)
add_subdirectory(log4cpp)
add_subdirectory(DBPlugin) 

set(PROTOBUFF_ROOT ../protobuf-3.9.0)
set(BOOST_ROOT ../boost_1_70_0)
# set(PYTHON_LIBRARY C:/Python34/libs)
set(PYTHON_ROOT Depends/Python38)
set(ZMQ_ROOT Depends/zmq)
find_package(Boost REQUIRED)
# find_package(PythonLibs REQUIRED)

link_directories(${Boost_LIBRARY_DIRS} ${PYTHON_ROOT}/libs log4cpp ${PROTOBUFF_ROOT}/cmake/build DBPlugin Depends/mysql/lib64/vs14 ${ZMQ_ROOT}/libs)
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_ROOT}/include log4cpp/include ${PROTOBUFF_ROOT}/src DBPlugin DBPlugin/include util ${ZMQ_ROOT}/include)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")
	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (NOT WIN32)

# if (MSVC)
#     foreach(flag_var
#         CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
#         CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
#       if(${flag_var} MATCHES "/MD")
#         string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
#       endif(${flag_var} MATCHES "/MD")
#     endforeach(flag_var)
#  endif (MSVC)

# link_directories(${Boost_LIBRARY_DIRS})
 
aux_source_directory(./GameServer MYSERVER_SRC_DIR)
aux_source_directory(./GameServer/proto MYSERVER_PROTO_SRC_DIR)
aux_source_directory(./GameServer/db MYSERVER_DB_DIR)
add_executable(GameServer ${MYSERVER_SRC_DIR} ${MYSERVER_PROTO_SRC_DIR} ${MYSERVER_DB_DIR})

target_link_libraries(GameServer MathFunction log4cpp DBPlugin mysqlcppconn libzmq-v142-mt-4_3_3)
target_link_libraries(GameServer debug libprotobufd optimized libprotobuf)

message(STATUS ${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(GameServer PROPERTIES DEBUG_POSTFIX "_d")
