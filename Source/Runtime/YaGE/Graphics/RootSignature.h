#pragma once

#include "../Core/Common.h"

#include <d3d12.h>
#include <wrl/client.h>

#include <bitset>

namespace YaGE {

class RootSignature {
public:
    /// @brief
    ///   Create a new root signature.
    ///
    /// @param[in] desc     Root signature description that describes how to create this root signature.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to serialize or create the root signature.
    YAGE_API RootSignature(const D3D12_ROOT_SIGNATURE_DESC &desc);

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
    ///   Get total number of non-sampler descriptors in descriptor tables.
    ///
    /// @return uint32_t
    ///   Return total number of non-sampler descriptors in descriptor tables.
    YAGE_NODISCARD auto TableDescriptorCount() const noexcept -> uint32_t { return tableDescriptorCount; }

    /// @brief
    ///   Get number of static samplers in this root signature.
    ///
    /// @return uint32_t
    ///   Return number of static samplers in this root signature.
    YAGE_NODISCARD auto StaticSamplerCount() const noexcept -> uint32_t { return staticSamplerCount; }

    /// @brief
    ///   Get total number of non-static sampler descriptors in this root signature.
    ///
    /// @return uint32_t
    ///   Return total number of non-static sampler descriptors in this root signature.
    YAGE_NODISCARD auto TableSamplerCount() const noexcept -> uint32_t { return samplerCount; }

    /// @brief
    ///   Checks if the specified root parameter is a non-sampler descriptor table.
    ///
    /// @param slot Root parameter index.
    ///
    /// @return
    /// @retval true  The specified root parameter is a non-sampler descriptor table.
    /// @retval false The specified root parameter is not a non-sampler descriptor table.
    YAGE_NODISCARD auto IsDescriptorTable(uint32_t slot) const noexcept -> bool { return descriptorTableFlags[slot]; }

    /// @brief
    ///   Checks if the specified root parameter is a sampler descriptor table.
    ///
    /// @param slot Root parameter index.
    ///
    /// @return
    /// @retval true  The specified root parameter is a sampler descriptor table.
    /// @retval false The specified root parameter is not a sampler descriptor table.
    YAGE_NODISCARD auto IsSamplerTable(uint32_t slot) const noexcept -> bool { return samplerTableFlags[slot]; }

    /// @brief
    ///   Get number of descriptors in the specified non-sampler descriptor table.
    ///
    /// @param slot Root parameter index.
    ///
    /// @return uint32_t
    ///   Return number of descriptors in the specified non-sampler descriptor table. Return 0 if the specified root parameter is not a root descriptor table.
    YAGE_NODISCARD auto NonSamplerDescriptorTableSize(uint32_t slot) const noexcept -> uint32_t {
        if (descriptorTableFlags[slot])
            return descriptorTableSizes[slot];
        return 0;
    }

    /// @brief
    ///   Get number of descriptors in the specified sampler table.
    ///
    /// @param slot Root parameter index.
    ///
    /// @return uint32_t
    ///   Return number of descriptors in the specified sampler table. Return 0 if the specified root parameter is not a sampler descriptor table.
    YAGE_NODISCARD auto SamplerTableSize(uint32_t slot) const noexcept -> uint32_t {
        if (samplerTableFlags[slot])
            return descriptorTableSizes[slot];
        return 0;
    }

    /// @brief
    ///   Get D3D12 root signature object.
    ///
    /// @return ID3D12RootSignature *
    ///   Return D3D12 root signature object.
    YAGE_NODISCARD auto D3D12RootSignature() const noexcept -> ID3D12RootSignature * { return rootSignature.Get(); }

private:
    /// @brief  D3D12 root signature object.
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    /// @brief  Total number of non-sampler descriptor count in descriptor tables.
    uint32_t tableDescriptorCount;

    /// @brief  Number of static samplers.
    uint32_t staticSamplerCount;

    /// @brief  Total number of non-static sampler descriptor count.
    uint32_t samplerCount;

    /// @brief  One bit is set for non-sampler root descriptor table.
    std::bitset<64> descriptorTableFlags;

    /// @brief  One bit is set for sampler root descriptor table.
    std::bitset<64> samplerTableFlags;

    /// @brief  Number of descriptors in each descriptor table.
    uint32_t descriptorTableSizes[64];
};

} // namespace YaGE
