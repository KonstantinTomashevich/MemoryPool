file (GLOB_RECURSE SOURCES *.cpp)
file (GLOB_RECURSE HEADERS *.hpp)

set (TARGET MemoryPoolBenchmark)
add_executable (${TARGET} ${SOURCES} ${HEADERS})
target_link_libraries (${TARGET} benchmark Boost::headers)