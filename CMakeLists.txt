cmake_minimum_required(VERSION 3.12.4)

set (CMAKE_CXX_STANDARD 17)

project (MemoryPool)

set (RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin" CACHE PATH "Directory where all runtime outputs will be stored.")
option (MEMORY_POOL_TESTS "Build MemoryPool library tests." OFF)
option (MEMORY_POOL_BENCHMARK "Build MemoryPool library benchmark." OFF)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}")
# Workaround for Visual Studio generator, that removes unnecessary Debug/Release directories.
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RUNTIME_OUTPUT_DIRECTORY}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RUNTIME_OUTPUT_DIRECTORY}")

# TODO: Implement library installation.
add_subdirectory (ThirdParty)
add_subdirectory (Source)

if (MEMORY_POOL_TESTS)
    add_subdirectory (Test)
endif ()

if (MEMORY_POOL_BENCHMARK)
    add_subdirectory (Benchmark)
endif ()