cmake_minimum_required(VERSION 3.12.4)

set (CMAKE_CXX_STANDARD 17)

project (MemoryPool)

message(STATUS "DEBUG BOOST DIR ${Boost_DIR}")
message(STATUS "DEBUG BOOST DIR ENV $ENV{Boost_DIR}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")
# Workaround for Visual Studio generator, that remove unnecessary Debug/Release directories.
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Bin")

add_subdirectory (ThirdParty)
add_subdirectory (Source)
add_subdirectory (Test)
add_subdirectory (Benchmark)