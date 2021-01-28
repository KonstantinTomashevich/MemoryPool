file (GLOB_RECURSE SOURCES *.cpp)
file (GLOB_RECURSE HEADERS *.hpp)

set (TARGET MemoryPoolBenchmark)
add_executable (${TARGET} ${SOURCES} ${HEADERS})
target_link_libraries (${TARGET} benchmark Boost::headers)

# Enable exceptions for clang cl (they are disabled by default), because boost pool uses them.
if (MSVC)
    target_compile_options (${TARGET} PUBLIC /EHa)
endif ()