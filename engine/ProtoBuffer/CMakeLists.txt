
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DPROTO_BUFFER_EXPORT)

include_directories(${PROTOBUFF_ROOT}/src)
link_directories(${PROTOBUFF_ROOT}/cmake/build)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRC *.cpp *cc)
add_library(ProtoBuffer SHARED ${CURRENT_HEADERS} ${CURRENT_SRC})
target_link_libraries(ProtoBuffer debug libprotobufd optimized libprotobuf)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
