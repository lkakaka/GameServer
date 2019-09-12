cmake_minimum_required(VERSION 3.0)
project(MyServer)
add_subdirectory(Math)
add_subdirectory(log4cpp)

link_directories(log4cpp/Debug)
include_directories(log4cpp/include)

add_executable(MyServer ./MyServer/ServerMain.cpp)
target_link_libraries(MyServer MathFunction log4cpp)
