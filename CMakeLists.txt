cmake_minimum_required(VERSION 3.0)
project(MyServer)

message(STATUS ${PROJECT_SOURCE_DIR})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(DEPENDS_ROOT ${PROJECT_SOURCE_DIR}/Depends)
set(LOG4CPP_ROOT ${PROJECT_SOURCE_DIR}/log4cpp)
set(ZMQ_ROOT ${DEPENDS_ROOT}/zmq)

add_subdirectory(Math)
add_subdirectory(log4cpp)
add_subdirectory(DBPlugin)
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
