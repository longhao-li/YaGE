cmake_minimum_required(VERSION 3.14)

project(YaGE LANGUAGES CXX VERSION 0.0.1)

# disable in-source build.
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source build is not allowed.")
endif()

# Build options.
option(YAGE_BUILD_EXAMPLES    "Build examples." OFF)
option(YAGE_BUILD_SHARED_LIBS "Build YaGE runtime as shared library." OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Add module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")

# Build projects.
add_subdirectory(Source)
