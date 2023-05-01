# Build YaGE runtime.
add_subdirectory(Runtime)

# Build examples.
if(YAGE_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()
