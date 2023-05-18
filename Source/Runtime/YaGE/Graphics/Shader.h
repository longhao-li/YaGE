#pragma once

#include "../Core/StringView.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

namespace YaGE {

enum class ShaderCompileOption {
    None                           = 0,
    Debug                          = D3DCOMPILE_DEBUG,
    SkipOptimization               = D3DCOMPILE_SKIP_OPTIMIZATION,
    Strict                         = D3DCOMPILE_ENABLE_STRICTNESS,
    UseRowMajorMatrix              = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
    UseColumnMajorMatrix           = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR,
    OptimizeLevel0                 = D3DCOMPILE_OPTIMIZATION_LEVEL0,
    OptimizeLevel1                 = D3DCOMPILE_OPTIMIZATION_LEVEL1,
    OptimizeLevel2                 = D3DCOMPILE_OPTIMIZATION_LEVEL2,
    OptimizeLevel3                 = D3DCOMPILE_OPTIMIZATION_LEVEL3,
    WarningAsError                 = D3DCOMPILE_WARNINGS_ARE_ERRORS,
    EnableUnboundedDescriptorTable = D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES,
};

constexpr auto operator|(ShaderCompileOption lhs, ShaderCompileOption rhs) noexcept -> ShaderCompileOption {
    return static_cast<ShaderCompileOption>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

constexpr auto operator&(ShaderCompileOption lhs, ShaderCompileOption rhs) noexcept -> ShaderCompileOption {
    return static_cast<ShaderCompileOption>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

constexpr auto operator^(ShaderCompileOption lhs, ShaderCompileOption rhs) noexcept -> ShaderCompileOption {
    return static_cast<ShaderCompileOption>(static_cast<int>(lhs) ^ static_cast<int>(rhs));
}

constexpr auto operator~(ShaderCompileOption flags) noexcept -> ShaderCompileOption {
    return static_cast<ShaderCompileOption>(~static_cast<int>(flags));
}

constexpr auto operator|=(ShaderCompileOption &lhs, ShaderCompileOption rhs) noexcept -> ShaderCompileOption & {
    lhs = lhs | rhs;
    return lhs;
}

constexpr auto operator&=(ShaderCompileOption &lhs, ShaderCompileOption rhs) noexcept -> ShaderCompileOption & {
    lhs = lhs & rhs;
    return lhs;
}

constexpr auto operator^=(ShaderCompileOption &lhs, ShaderCompileOption rhs) noexcept -> ShaderCompileOption & {
    lhs = lhs ^ rhs;
    return lhs;
}

enum class ShaderType {
    Vertex,
    Pixel,
    Geometry,
    Hull,
    Domain,
    Compute,
};

class Shader {
public:
    /// @brief
    ///   Create an empty shader.
    Shader() noexcept;

    /// @brief
    ///   Copy constructor for Shader. Copy on write is used.
    ///
    /// @param other    The shader object to be copied from.
    Shader(const Shader &other) noexcept = default;

    /// @brief
    ///   Copy assignment for Shader. Copy on write is used.
    ///
    /// @param other    The shader object to be copied from.
    auto operator=(const Shader &other) noexcept -> Shader & = default;

    /// @brief
    ///   Move constructor for Shader. The moved shader object will be empty.
    ///
    /// @param other    The shader object to be moved from.
    Shader(Shader &&other) noexcept = default;

    /// @brief
    ///   Move assignment for Shader. The moved shader object will be empty.
    ///
    /// @param other    The shader object to be moved from.
    auto operator=(Shader &&other) noexcept -> Shader & = default;

    /// @brief
    ///   Release this shader data.
    ~Shader() noexcept;

    /// @brief
    ///   Try to compile a shader from file. Shader model 6.0 is used.
    ///
    /// @param path             The path to the HLSL shader file.
    /// @param shaderType       Type of the shader.
    /// @param entryPoint       Entry point name of the shader.
    /// @param compileOptions   Shader compile options.
    ///
    /// @return bool
    /// @retval true    Compilation succeeded.
    /// @retval false   Compilation failed.
    YAGE_API auto CompileFromFile(StringView          path,
                                  ShaderType          shaderType,
                                  StringView          entryPoint,
                                  ShaderCompileOption compileOptions) noexcept -> bool;

    /// @brief
    ///   Try to compile a shader from string. Shader model 6.0 is used.
    ///
    /// @param src              Pointer to the HLSL shader string. The HLSL source string should be a ASCII string.
    /// @param size             Size in byte of the HLSL shader string.
    /// @param shaderType       Type of the shader.
    /// @param entryPoint       Entry point name of the shader.
    /// @param compileOptions   Shader compile options.
    ///
    /// @return bool
    /// @retval true    Compilation succeeded.
    /// @retval false   Compilation failed.
    YAGE_API auto CompileFromString(const void         *src,
                                    size_t              size,
                                    ShaderType          shaderType,
                                    StringView          entryPoint,
                                    ShaderCompileOption compileOptions) noexcept -> bool;

    /// @brief
    ///   Try to load a shader from a compiled shader binary file.
    ///
    /// @param path     The path to the compiled shader binary file.
    ///
    /// @return bool
    /// @retval true    Loading succeeded.
    /// @retval false   Loading failed.
    YAGE_API auto LoadBinary(StringView path) noexcept -> bool;

    /// @brief
    ///   Get pointer to shader data.
    ///
    /// @return const void *
    ///   Return @p nullptr if the shader is empty. Otherwise, return pointer to start of the shader.
    YAGE_NODISCARD auto Data() const noexcept -> const void * {
        return (shaderBinary ? shaderBinary->GetBufferPointer() : nullptr);
    }

    /// @brief
    ///   Get size in byte of this shader.
    ///
    /// @return size_t
    ///   Return 0 if the shader is empty. Otherwise, return size in byte of this shader.
    YAGE_NODISCARD auto Size() const noexcept -> size_t { return (shaderBinary ? shaderBinary->GetBufferSize() : 0); }

private:
    /// @brief
    ///   Binary data of this shader.
    Microsoft::WRL::ComPtr<ID3DBlob> shaderBinary;
};

} // namespace YaGE
