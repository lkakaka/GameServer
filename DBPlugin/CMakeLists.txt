
set(CMAKE_DEBUG_POSTFIX "_d") 
#aux_source_directory(. DIR_LIB_SRCS)
file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_definitions(-DSERVER_EXPORT)

if (CMAKE_COMPILER_IS_GNUCXX)
 	set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
 	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories(../log4cpp/include ../Depends/mysql/include include ${PYTHON_ROOT}/include)
link_directories(../Depends/mysql/lib64 ${PYTHON_ROOT}/libs)
link_libraries(log4cpp mysqlcppconn)
add_library(DBPlugin SHARED ${CURRENT_HEADERS} ${CURRENT_SRC} ${COMMON_SRC})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
