set (CELERO_ENABLE_EXPERIMENTS FALSE)
add_subdirectory (Celero)

# Override Celero output directory, so it will be stored with other executables.
set_target_properties(celero PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")