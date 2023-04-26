#ifndef YAGE_GRAPHICS_DYNAMIC_DESCRIPTOR_HEAP_H
#define YAGE_GRAPHICS_DYNAMIC_DESCRIPTOR_HEAP_H

#include "Descriptor.h"
#include "RootSignature.h"

#include <vector>

namespace YaGE {

class DynamicDescriptorHeap {
private:
    enum class ParameterType {
        None = 0,
        DescriptorHandle,
        ConstantBufferView,
    };

    /// @note
    ///   This is a descriptor handle if @p desc.SizeInBytes is 0.
    struct CachedParameter {
        ParameterType parameterType;
        union {
            D3D12_CPU_DESCRIPTOR_HANDLE     descriptorHandle;
            D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferView;
        } parameter;
    };

    struct DescriptorTableCache {
        CachedParameter *parameters;
        uint32_t         parameterCount;
    };

public:
    /// @brief
    ///   Create a new dynamic descriptor heap for the specified type of descriptor.
    ///
    /// @param descriptorType   Type of this descriptor heap. Must be @p D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV or @p D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to acquire D3D12 device.
    YAGE_API DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE descriptorType);

    /// @brief
    ///   Copy constructor is disabled.
    DynamicDescriptorHeap(const DynamicDescriptorHeap &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const DynamicDescriptorHeap &) = delete;

    /// @brief
    ///   Destroy this dynamic descriptor heap.
    YAGE_API ~DynamicDescriptorHeap() noexcept;

    /// @brief
    ///   Clean up all descriptors and root signatures.
    ///
    /// @param syncPoint    The sync point that indicates when the retired descriptor heaps could be reused.
    YAGE_API auto CleanUp(uint64_t syncPoint) noexcept -> void;

    /// @brief
    ///   Parse the specified graphics root signature and prepare for binding descriptors.
    ///
    /// @param[in] rootSig  The root signature to be parsed.
    YAGE_API auto ParseGraphicsRootSignature(RootSignature &rootSig) noexcept -> void;

    /// @brief
    ///   Bind a graphics descriptor to the specified descriptor table.
    ///
    /// @param paramIndex   Root parameter index of the descriptor table in the root signature.
    /// @param offset       Offset of the descriptor from start of the descriptor table.
    /// @param descriptor   Descriptor to bind.
    YAGE_API auto BindGraphicsDescriptor(uint32_t paramIndex, uint32_t offset, CpuDescriptorHandle descriptor) noexcept
        -> void;

    /// @brief
    ///   Create a new constant buffer view at the specified offset of the descriptor table.
    ///
    /// @param paramIndex   Root parameter index of the descriptor table in the root signature.
    /// @param offset       Offset of the descriptor from start of the descriptor table.
    /// @param desc         Description of the constant buffer view.
    YAGE_API auto BindGraphicsDescriptor(uint32_t                               paramIndex,
                                         uint32_t                               offset,
                                         const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) noexcept -> void;

    /// @brief
    ///   Parse the specified compute root signature and prepare for binding descriptors.
    ///
    /// @param[in] rootSig  The root signature to be parsed.
    YAGE_API auto ParseComputeRootSignature(RootSignature &rootSig) noexcept -> void;

    /// @brief
    ///   Bind a compute descriptor to the specified descriptor table.
    ///
    /// @param paramIndex   Root parameter index of the descriptor table in the root signature.
    /// @param offset       Offset of the descriptor from start of the descriptor table.
    /// @param descriptor   Descriptor to bind.
    YAGE_API auto BindComputeDescriptor(uint32_t paramIndex, uint32_t offset, CpuDescriptorHandle descriptor) noexcept
        -> void;

    /// @brief
    ///   Create a new constant buffer view at the specified offset of the descriptor table.
    ///
    /// @param paramIndex   Root parameter index of the descriptor table in the root signature.
    /// @param offset       Offset of the descriptor from start of the descriptor table.
    /// @param desc         Description of the constant buffer view.
    YAGE_API auto BindComputeDescriptor(uint32_t                               paramIndex,
                                        uint32_t                               offset,
                                        const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) noexcept -> void;

    /// @brief
    ///   Upload all descriptors in the cached descriptor tables to GPU. This method will also bind the descriptor heap to the command list.
    ///
    /// @param cmdList  Command list to upload descriptors.
    YAGE_API auto Commit(ID3D12GraphicsCommandList *cmdList) noexcept -> void;

private:
    /// @brief  Device that is used to create and copy descriptors.
    ID3D12Device *const device;

    /// @brief  Descriptor type of this dynamic descriptor heap.
    const D3D12_DESCRIPTOR_HEAP_TYPE descriptorType;

    /// @brief  Descriptor increment size.
    const uint32_t descriptorSize;

    /// @brief  Current graphics root signature.
    RootSignature *graphicsRootSignature;

    /// @brief  Current compute root signature.
    RootSignature *computeRootSignature;

    /// @brief  Current dynamic descriptor heap.
    ID3D12DescriptorHeap *currentHeap;

    /// @brief  Descriptor handle to next allocation position.
    DescriptorHandle currentHandle;

    /// @brief  Number of free descriptors in current descriptor heap.
    uint32_t freeDescriptorCount;

    /// @brief  Retired descriptor heaps.
    std::vector<ID3D12DescriptorHeap *> retiredHeaps;

    /// @brief  Cached parameters for graphics root signature.
    std::vector<CachedParameter> graphicsCachedParameters;

    /// @brief  Cached parameters for compute root signature.
    std::vector<CachedParameter> computeCachedParameters;

    /// @brief  Cached descriptor tables for graphics root signature.
    DescriptorTableCache graphicsTableCache[64];

    /// @brief  Cached descriptor tables for compute root signature.
    DescriptorTableCache computeTableCache[64];
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_DYNAMIC_DESCRIPTOR_HEAP_H
