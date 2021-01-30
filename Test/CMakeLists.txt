include_directories (${CMAKE_SOURCE_DIR}/Source)
find_package (Boost COMPONENTS unit_test_framework REQUIRED)
add_subdirectory (Memory)