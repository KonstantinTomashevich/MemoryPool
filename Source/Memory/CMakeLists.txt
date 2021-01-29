file (GLOB_RECURSE SOURCES *.cpp)
file (GLOB_RECURSE HEADERS *.hpp)

set (TARGET Memory)
add_library (${TARGET} ${SOURCES} ${HEADERS})