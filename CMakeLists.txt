cmake_minimum_required(VERSION 3.0)
project(MyServer)
add_subdirectory(Math)
add_subdirectory(log4cpp)

link_directories(log4cpp/Debug ../protobuf/cmake)
include_directories(log4cpp/include ../protobuf/src)

if (WIN32)
	set(BOOST_ROOT ../boost_1_70_0)
else (WIN32)
	set(BOOST_ROOT ../boost_1_70_0)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")
	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (WIN32)

if (False)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
 endif (MSVC)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

aux_source_directory(./MyServer MYSERVER_SRC_DIR)
aux_source_directory(./MyServer/proto MYSERVER_PROTO_SRC_DIR)

add_executable(MyServer ${MYSERVER_SRC_DIR} ${MYSERVER_PROTO_SRC_DIR})
target_link_libraries(MyServer MathFunction log4cpp libprotobufd)
