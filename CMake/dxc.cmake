cmake_minimum_required(VERSION 3.14)

# YaGEAddGraphicsShaderTarget
#
# Add a custom target to compile graphics shader using dxc.exe. You may need Visual Studio developer command prompt to use this function. Shader model 6.0 is used here.
# Default shader output directory is ${CMAKE_CURRENT_BINARY_DIR}/Shaders. You can change it by setting YAGE_SHADER_OUTPUT_DIR.
#
# Usage:
#   YaGEAddGraphicsShaderTarget(targetName VERTEX vertexShader1.hlsl vertexShader2.hlsl ... PIXEL pixelShader1.hlsl pixelShader2.hlsl ...)
function(YaGEAddGraphicsShaderTarget targetName)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(dxcExtraFlags "-Zi" "-Od" "-Qembed_debug")
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(dxcExtraFlags "-Zi" "-O2" "-Qembed_debug")
    else()
        set(dxcExtraFlags "-O3")
    endif()

    # No shader resource given.
    if(NOT ${ARGC} GREATER 1)
        message(FATAL_ERROR "No shader source file given.")
    endif()

    set(shaderType "VERTEX")
    set(shaderBinaryList)

    set(index 1)
    while(index LESS ${ARGC})
        set(currentSource ${ARGV${index}})
        math(EXPR index "${index} + 1")

        # Update shader type.
        if(currentSource MATCHES "VERTEX|PIXEL|DOMAIN|HULL|GEOMETRY")
            set(shaderType ${currentSource})
            continue()
        endif()

        if(shaderType STREQUAL "VERTEX")
            set(shaderModel "vs_6_0")
            set(shaderEntry "VertexMain")
            set(shaderBinarySuffix ".vso")
            set(promptShaderType "vertex")
        elseif(shaderType STREQUAL "PIXEL")
            set(shaderModel "ps_6_0")
            set(shaderEntry "PixelMain")
            set(shaderBinarySuffix ".pso")
            set(promptShaderType "pixel")
        elseif(shaderType STREQUAL "DOMAIN")
            set(shaderModel "ds_6_0")
            set(shaderEntry "DomainMain")
            set(shaderBinarySuffix ".dso")
            set(promptShaderType "domain")
        elseif(shaderType STREQUAL "HULL")
            set(shaderModel "hs_6_0")
            set(shaderEntry "HullMain")
            set(shaderBinarySuffix ".hso")
            set(promptShaderType "hull")
        elseif(shaderType STREQUAL "GEOMETRY")
            set(shaderModel "gs_6_0")
            set(shaderEntry "GeometryMain")
            set(shaderBinarySuffix ".gso")
            set(promptShaderType "geometry")
        else()
            message(FATAL_ERROR "Unknown shader type: ${shaderType}")
        endif()

        # Create directory if needed.
        if(YAGE_SHADER_OUTPUT_DIR)
            set(shaderOutputDir ${YAGE_SHADER_OUTPUT_DIR})
        else()
            set(shaderOutputDir ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
        endif()

        # Check source path.
        if(IS_ABSOLUTE ${currentSource})
            set(shaderSource ${currentSource})
        else()
            set(shaderSource ${CMAKE_CURRENT_SOURCE_DIR}/${currentSource})
        endif()

        # Get output path.
        get_filename_component(shaderName ${shaderSource} NAME_WE)
        set(shaderBinary ${shaderOutputDir}/${shaderName}${shaderBinarySuffix})

        # Compile shader.
        add_custom_command(
            OUTPUT          ${shaderBinary}
            COMMAND         dxc -T ${shaderModel} -E ${shaderEntry} -Fo ${shaderBinary} ${dxcExtraFlags} ${shaderSource}
            MAIN_DEPENDENCY ${shaderSource}
            COMMENT         "Compiling ${promptShaderType} shader: ${shaderSource}"
        )

        list(APPEND shaderBinaryList ${shaderBinary})
    endwhile()

    add_custom_target(${targetName} ALL DEPENDS ${shaderBinaryList})
    
    # Create directory if needed.
    add_custom_command(
        TARGET ${targetName} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${shaderOutputDir}
        COMMENT "Creating shader output directory: ${shaderOutputDir}"
    )
endfunction()
