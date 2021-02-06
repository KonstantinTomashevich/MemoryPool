cmake_minimum_required(VERSION 3.12.4)

set (CMAKE_CXX_STANDARD 17)

project (MemoryPool)

option (RUNTIME_OUTPUT_DIRECTORY "Directory where all runtime outputs will be stored." "${CMAKE_BINARY_DIR}/Bin")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}")
# Workaround for Visual Studio generator, that removes unnecessary Debug/Release directories.
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RUNTIME_OUTPUT_DIRECTORY}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RUNTIME_OUTPUT_DIRECTORY}")

# TODO: Add options to skip tests and benchmark builds.
# TODO: Implement library installation.
add_subdirectory (ThirdParty)
add_subdirectory (Source)
add_subdirectory (Test)
add_subdirectory (Benchmark)