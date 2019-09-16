cmake_minimum_required(VERSION 3.0)
project(MyServer)
add_subdirectory(Math)
add_subdirectory(log4cpp)

link_directories(log4cpp/Debug)
include_directories(log4cpp/include)

if (WIN32)
	set(BOOST_ROOT F:/Project/boost_1_70_0)
else (WIN32)
	set(BOOST_ROOT ../boost_1_70_0)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")
	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (WIN32)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

aux_source_directory(./MyServer MYSERVER_SRC_DIR)

add_executable(MyServer ${MYSERVER_SRC_DIR})
target_link_libraries(MyServer MathFunction log4cpp)
