file (GLOB_RECURSE SOURCES *.cpp)
file (GLOB_RECURSE HEADERS *.hpp)

add_executable (MemoryPoolBenchmark ${SOURCES} ${HEADERS})
add_dependencies (MemoryPoolBenchmark celero)
target_link_libraries (MemoryPoolBenchmark celero)