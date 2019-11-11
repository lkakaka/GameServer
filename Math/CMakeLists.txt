aux_source_directory(. DIR_LIB_SRCS)
add_definitions(-DMATH_EXPORT)
add_library(MathFunction SHARED ${DIR_LIB_SRCS})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)