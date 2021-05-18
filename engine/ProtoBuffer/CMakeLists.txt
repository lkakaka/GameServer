
set(CMAKE_DEBUG_POSTFIX "_d")

#add_definitions(-DPROTO_BUFFER_EXPORT)

gnu_cxx_flag("-fPIC")

include_directories(${PROTOBUFF_ROOT}/include ${PROTO_ROOT})
link_directories(${PROTOBUFF_ROOT}/libs)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(ProtoBuffer ${CURRENT_HEADERS} ${CURRENT_SRC})
if(WIN32)
	target_link_libraries(ProtoBuffer debug libprotobufd optimized libprotobuf)
else()
	target_link_libraries(ProtoBuffer protobuf)
endif()
SET(LIBRARY_OUTPUT_PATH ${PROJECT_OUT_PATH})
