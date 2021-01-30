file (GLOB_RECURSE SOURCES *.cpp)
file (GLOB_RECURSE HEADERS *.hpp)

set (TARGET MemoryTest)
add_executable (${TARGET} ${SOURCES} ${HEADERS})
target_link_libraries (${TARGET} Memory Boost::unit_test_framework)