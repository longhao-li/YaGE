#ifndef YAGE_GRAPHICS_ROOT_SIGNATURE_H
#define YAGE_GRAPHICS_ROOT_SIGNATURE_H

#include "../Core/Common.h"

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>

namespace YaGE {

struct RootSignatureInfo {
    /// @brief
    ///   Create an empty graphics root signature info.
    RootSignatureInfo() noexcept
        : constantBufferViewCount(), shaderResourceViewCount(), unorderedAccessViewCount(), samplerViewCount() {}

    /// @brief
    ///   Add constant buffer views for the specified shader space.
    ///
    /// @param space    The specified shader space to add constant buffer views for. This value should not be greater than 16.
    /// @param count    Number of constant buffer views to add.
    auto AddConstantBufferView(uint32_t space, uint32_t count) noexcept -> RootSignatureInfo & {
        constantBufferViewCount[space] += count;
        return *this;
    }

    /// @brief
    ///   Add shader resource views for the specified shader space.
    ///
    /// @param space    The specified shader space to add shader resource views for. This value should not be greater than 16.
    /// @param count    Number of shader resource views to add.
    auto AddShaderResourceView(uint32_t space, uint32_t count) noexcept -> RootSignatureInfo & {
        shaderResourceViewCount[space] += count;
        return *this;
    }

    /// @brief
    ///   Add unordered access views for the specified shader space.
    ///
    /// @param space   The specified shader space to add unordered access views for. This value should not be greater than 16.
    /// @param count   Number of unordered access views to add.
    auto AddUnorderedAccessView(uint32_t space, uint32_t count) noexcept -> RootSignatureInfo & {
        unorderedAccessViewCount[space] += count;
        return *this;
    }

    /// @brief
    ///   Add sampler views for the specified shader space.
    ///
    /// @param space    The specified shader space to add sampler views for.
    /// @param count    Number of sampler views to add.
    auto AddSamplerView(uint32_t space, uint32_t count) noexcept -> RootSignatureInfo & {
        samplerViewCount[space] += count;
        return *this;
    }

    /// @brief  Constant buffer view count for each space.
    uint32_t constantBufferViewCount[16];

    /// @brief  Shader resource view count for each space.
    uint32_t shaderResourceViewCount[16];

    /// @brief  Unordered access view count for each space.
    uint32_t unorderedAccessViewCount[16];

    /// @brief  Sampler view count for each space.
    uint32_t samplerViewCount[16];
};

class RootSignature {
public:
    /// @brief
    ///   Create a root signature from the specified root signature info.
    ///
    /// @param info     Root signature info that describes how to create this root signature.
    /// @param flags    Root signature flags.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to serialize and create this root signature.
    YAGE_API RootSignature(const RootSignatureInfo &info, D3D12_ROOT_SIGNATURE_FLAGS flags);

    /// @brief
    ///   Copy constructor is disabled.
    RootSignature(const RootSignature &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const RootSignature &) = delete;

    /// @brief
    ///   Destroy this root signature.
    YAGE_API ~RootSignature() noexcept;

    /// @brief
    ///   Get total number of CBV/SRV/UAV descriptors in this root signature.
    ///
    /// @return uint32_t
    ///   Return total number of CBV/SRV/UAV descriptors in this root signature.
    YAGE_NODISCARD auto DescriptorCount() const noexcept -> uint32_t {
        return constantBufferViewCount + shaderResourceViewCount + unorderedAccessViewCount;
    }

    /// @brief
    ///   Get total number of sampler descriptors in this root signature.
    ///
    /// @return uint32_t
    ///   Return total number of sampler descriptors in this root signature.
    YAGE_NODISCARD auto SamplerCount() const noexcept -> uint32_t { return samplerViewCount; }

    /// @brief
    ///   Get the D3D12 root signature object.
    ///
    /// @return ID3D12RootSignature *
    ///   Return D3D12 root signature object.
    YAGE_NODISCARD auto D3D12RootSignature() const noexcept -> ID3D12RootSignature * { return rootSignature.Get(); }

    friend class DynamicDescriptorHeap;

private:
    /// @brief  D3D12 root signature object.
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    /// @brief  Offset from start of root signature in each descriptor table.
    uint32_t constantBufferViewOffset[16];

    /// @brief  Total number of constant buffer views.
    uint32_t constantBufferViewCount;

    /// @brief  Offset from start of root signature in each descriptor table.
    uint32_t shaderResourceViewOffset[16];

    /// @brief  Total number of shader resource views.
    uint32_t shaderResourceViewCount;

    /// @brief  Offset from start of root signature in each descriptor table.
    uint32_t unorderedAccessViewOffset[16];

    /// @brief  Total number of unordered access views.
    uint32_t unorderedAccessViewCount;

    /// @brief  Offset from start of root signature in each descriptor table.
    uint32_t samplerViewOffset[16];

    /// @brief  Total number of sampler views.
    uint32_t samplerViewCount;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_ROOT_SIGNATURE_H
