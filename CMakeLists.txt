cmake_minimum_required(VERSION 3.0)
project(MyServer)

message(STATUS ${PROJECT_SOURCE_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(DEPENDS_ROOT ${PROJECT_SOURCE_DIR}/Depends)
set(LOG4CPP_ROOT ${PROJECT_SOURCE_DIR}/log4cpp)
set(ZMQ_ROOT ${DEPENDS_ROOT}/zmq)
set(PYTHON_ROOT ${DEPENDS_ROOT}/Python38)
set(GAMEUTIL_ROOT ${PROJECT_SOURCE_DIR}/GameUtil)
set(PROTO_ROOT ${PROJECT_SOURCE_DIR}/ProtoBuffer)

set(BOOST_ROOT ../boost_1_70_0)
set(PROTOBUFF_ROOT ../../protobuf-3.9.0)
find_package(Boost REQUIRED)

file(GLOB_RECURSE PROTO_SRC ProtoBuffer/*.cpp ProtoBuffer/*.cc ProtoBuffer/*.h ProtoBuffer/*.hpp)

message(STATUS ${PROTO_SRC})

add_subdirectory(Math)
add_subdirectory(log4cpp)
add_subdirectory(GameUtil)
add_subdirectory(DBPlugin)
#add_subdirectory(ProtoBuffer)
add_subdirectory(Gateway)
add_subdirectory(SceneServer)
add_subdirectory(GameServer)
add_subdirectory(ZmqRouter)

add_subdirectory(TestZmq)

# if (NOT WIN32)
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")
# 	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
# endif (NOT WIN32)

# if (MSVC)
#     foreach(flag_var
#         CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
#         CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
#       if(${flag_var} MATCHES "/MD")
#         string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
#       endif(${flag_var} MATCHES "/MD")
#     endforeach(flag_var)
#  endif (MSVC)
