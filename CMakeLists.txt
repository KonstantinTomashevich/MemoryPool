cmake_minimum_required(VERSION 3.12.4)

set (CMAKE_CXX_STANDARD 17)

project (MemoryPool)

message (STATUS "DEBUG BOOST ROOT: $ENV{BOOST_ROOT}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")

add_subdirectory (ThirdParty)
add_subdirectory (Source)
add_subdirectory (Test)
add_subdirectory (Benchmark)