
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DSERVER_EXPORT -DOPEN_PROFILE_TRACK)

gnu_setting(true)

include_directories(${LOG4CPP_ROOT}/include ${Boost_INCLUDE_DIRS} ${PROTOBUFF_ROOT}/include ${GAMELIB_ROOT} ${ZMQ_ROOT}/include ${PROTO_ROOT})
link_directories(${LOG4CPP_ROOT} ${Boost_LIBRARY_DIRS} ${PROTOBUFF_ROOT}/libs ${GAMELIB_ROOT} ${ZMQ_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
#add_executable(ServiceCenter ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC})
#add_library(ServiceCenter ${CURRENT_HEADERS} ${CURRENT_SRC} ${PROTO_SRC})
add_library(ServiceCenter ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(ServiceCenter log4cpp GameLib ProtoBuffer)
if(WIN32)
	target_link_libraries(ServiceCenter debug libprotobufd optimized libprotobuf)
else()
	target_link_libraries(ServiceCenter protobuf)
endif()

SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
set_target_properties(ServiceCenter PROPERTIES DEBUG_POSTFIX "_d")
