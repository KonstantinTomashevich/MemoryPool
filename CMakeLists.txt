cmake_minimum_required(VERSION 3.12.4)

project (MemoryPool)

# Only for comparison with Boost.Pool.
find_package (Boost COMPONENTS headers REQUIRED)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")

add_subdirectory (ThirdParty)
add_subdirectory (Benchmark)