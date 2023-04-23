# Set target name.
set(YAGE_TARGET_NAME "YaGE")

# Add external libraries.
include("CPM")

CPMAddPackage(
    NAME              "fmt"
    GIT_TAG           "9.1.0"
    GITHUB_REPOSITORY "fmtlib/fmt"
    OPTIONS           "BUILD_SHARED_LIBS OFF"
                      "FMT_MASTER_PROJECT OFF"
                      "FMT_INSTALL OFF"
)

# Collect source files.
file(GLOB_RECURSE YAGE_HEADER_FILES "*.h")
file(GLOB_RECURSE YAGE_SOURCE_FILES "*.cpp")

# Build target.
if(YAGE_BUILD_SHARED_LIBS)
    add_library(${YAGE_TARGET_NAME} SHARED ${YAGE_HEADER_FILES} ${YAGE_SOURCE_FILES})
    target_compile_definitions(${YAGE_TARGET_NAME} PRIVATE "YAGE_BUILD_DLL" PUBLIC "YAGE_DLL")
else()
    add_library(${YAGE_TARGET_NAME} STATIC ${YAGE_HEADER_FILES} ${YAGE_SOURCE_FILES})
endif()

# Set compiler options.
if(MSVC)
    # MSVC and clang-cl. We must check clang-cl before clang, because passing "-Wall" to clang-cl is equal to passing "-Weverything" to clang.
    target_compile_options(${YAGE_TARGET_NAME} PRIVATE "/permissive-" "/W4" "/volatile:iso")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${YAGE_TARGET_NAME} PRIVATE "/utf-8")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    # Actually apple clang could never be used on Windows.
    target_compile_options(${YAGE_TARGET_NAME} PRIVATE "-Wall" "-Wextra" "-Wmost" "-Wshadow" "-Wredundant-decls" "-Wcast-align" "-fvisibility=hidden")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # It is not guaranteed that this project works with MinGW.
    target_compile_options(${YAGE_TARGET_NAME} PRIVATE "-Wall" "-Wextra" "-Wcast-align" "-Wno-cast-function-type" "-Wredundant-decls" "-fvisibility=hidden")
endif()

# Add definitions.
target_compile_definitions(${YAGE_TARGET_NAME} PRIVATE "WIN32_LEAN_AND_MEAN" "_CRT_SECURE_NO_WARNINGS" "UNICODE")

# Set include directory.
target_include_directories(${YAGE_TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Link libraries.
target_link_libraries(${YAGE_TARGET_NAME} PUBLIC fmt::fmt "d3d12" "dxgi")
