#include "Shader.h"
#include "../Core/String.h"

#include <Windows.h>

using namespace YaGE;
using Microsoft::WRL::ComPtr;

YaGE::Shader::Shader() noexcept : shaderBinary() {}

YaGE::Shader::~Shader() noexcept {}

auto YaGE::Shader::CompileFromFile(StringView          path,
                                   ShaderType          shaderType,
                                   StringView          entryPoint,
                                   ShaderCompileOption compileOptions) noexcept -> bool {
    const char *shaderModel = "vs_6_0";
    switch (shaderType) {
    case ShaderType::Vertex:
        shaderModel = "vs_6_0";
        break;
    case ShaderType::Pixel:
        shaderModel = "ps_6_0";
        break;
    case ShaderType::Geometry:
        shaderModel = "gs_6_0";
        break;
    case ShaderType::Hull:
        shaderModel = "hs_6_0";
        break;
    case ShaderType::Domain:
        shaderModel = "ds_6_0";
        break;
    case ShaderType::Compute:
        shaderModel = "cs_6_0";
        break;
    }

    std::string entry;
    entry.resize(entryPoint.Length() * 3 + 1);
    int count = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWSTR>(entryPoint.Data()),
                                    static_cast<int>(entryPoint.Length()), const_cast<LPSTR>(entry.data()),
                                    static_cast<int>(entry.capacity()), nullptr, nullptr);
    if (count <= 0)
        return false;
    entry[count] = '\0';

    ComPtr<ID3DBlob> binary;
    HRESULT          hr;

    if (path.IsNullTerminated()) {
        hr = D3DCompileFromFile(reinterpret_cast<LPCWSTR>(path.Data()), nullptr, nullptr, entry.data(), shaderModel,
                                static_cast<UINT>(compileOptions), 0, binary.GetAddressOf(), nullptr);
    } else {
        String temp(path);
        hr = D3DCompileFromFile(reinterpret_cast<LPCWSTR>(temp.Data()), nullptr, nullptr, entry.data(), shaderModel,
                                static_cast<UINT>(compileOptions), 0, binary.GetAddressOf(), nullptr);
    }

    if (FAILED(hr))
        return false;

    shaderBinary = std::move(binary);
    return true;
}

auto YaGE::Shader::CompileFromString(const void         *src,
                                     size_t              size,
                                     ShaderType          shaderType,
                                     StringView          entryPoint,
                                     ShaderCompileOption compileOptions) noexcept -> bool {
    const char *shaderModel = "vs_6_0";
    switch (shaderType) {
    case ShaderType::Vertex:
        shaderModel = "vs_6_0";
        break;
    case ShaderType::Pixel:
        shaderModel = "ps_6_0";
        break;
    case ShaderType::Geometry:
        shaderModel = "gs_6_0";
        break;
    case ShaderType::Hull:
        shaderModel = "hs_6_0";
        break;
    case ShaderType::Domain:
        shaderModel = "ds_6_0";
        break;
    case ShaderType::Compute:
        shaderModel = "cs_6_0";
        break;
    }

    std::string entry;
    entry.resize(entryPoint.Length() * 3 + 1);
    int count = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWSTR>(entryPoint.Data()),
                                    static_cast<int>(entryPoint.Length()), const_cast<LPSTR>(entry.data()),
                                    static_cast<int>(entry.capacity()), nullptr, nullptr);
    if (count <= 0)
        return false;
    entry[count] = '\0';

    ComPtr<ID3DBlob> binary;
    HRESULT          hr;

    hr = D3DCompile(src, size, nullptr, nullptr, nullptr, entry.data(), shaderModel, static_cast<UINT>(compileOptions),
                    0, binary.GetAddressOf(), nullptr);
    if (FAILED(hr))
        return false;

    shaderBinary = std::move(binary);
    return true;
}

auto YaGE::Shader::LoadBinary(StringView path) noexcept -> bool {
    ComPtr<ID3DBlob> binary;
    HRESULT          hr;

    if (path.IsNullTerminated()) {
        hr = D3DReadFileToBlob(reinterpret_cast<LPCWSTR>(path.Data()), binary.GetAddressOf());
    } else {
        String temp(path);
        hr = D3DReadFileToBlob(reinterpret_cast<LPCWSTR>(temp.Data()), binary.GetAddressOf());
    }

    if (FAILED(hr))
        return false;

    shaderBinary = std::move(binary);
    return true;
}
