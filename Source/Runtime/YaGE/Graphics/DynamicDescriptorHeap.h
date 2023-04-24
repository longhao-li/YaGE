#ifndef YAGE_GRAPHICS_DYNAMIC_DESCRIPTOR_HEAP_H
#define YAGE_GRAPHICS_DYNAMIC_DESCRIPTOR_HEAP_H

#include "Descriptor.h"
#include "RootSignature.h"

#include <vector>

namespace YaGE {

class RenderDevice;

class DynamicDescriptorHeap {
public:
    /// @brief
    ///   Create a new dynamic descriptor heap.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to acquire render device singleton.
    YAGE_API DynamicDescriptorHeap();

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
    ///   Set root signature.
    ///
    /// @param[in] rootSig  Root signature that is used to determine the descriptor tables.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to acquire new shader visible descriptor heaps.
    YAGE_API auto ParseRootSignature(RootSignature &rootSig) -> void;

    /// @brief
    ///   Reset this dynamic descriptor heap.
    ///
    /// @param syncPoint    Sync point that is used to determine when the freed descriptor heaps could be reused.
    YAGE_API auto Reset(uint64_t syncPoint) noexcept -> void;

    /// @brief
    ///   Bind descriptor tables and heap to graphics pipeline of the specified command list.
    ///
    /// @param cmdList  Command list that is used to bind descriptor tables and heaps.
    YAGE_API auto BindGraphics(ID3D12GraphicsCommandList *cmdList) noexcept -> void;

    /// @brief
    ///   Bind descriptor tables and heap to compute pipeline of the specified command list.
    ///
    /// @param cmdList  Command list that is used to bind descriptor tables and heaps.
    YAGE_API auto BindCompute(ID3D12GraphicsCommandList *cmdList) noexcept -> void;

    /// @brief
    ///   Bind constant buffer view to the given shader register.
    ///
    /// @param shaderSpace          Shader space to be bind to.
    /// @param shaderRegister       Shader register to be bind to.
    /// @param constantBufferView   Constant buffer view CPU handle to be bind to the given shader register.
    YAGE_API auto BindConstantBufferView(uint32_t            shaderSpace,
                                         uint32_t            shaderRegister,
                                         CpuDescriptorHandle constantBufferView) noexcept -> void;

    /// @brief
    ///   Create a new constant buffer view for the given shader register.
    ///
    /// @param shaderSpace      Shader space to be bind to.
    /// @param shaderRegister   Shader register to be bind to.
    /// @param gpuAddress       GPU address of the constant buffer.
    /// @param size             Size in byte of the constant buffer.
    YAGE_API auto
    BindConstantBufferView(uint32_t shaderSpace, uint32_t shaderRegister, uint64_t gpuAddress, uint32_t size) noexcept
        -> void;

    /// @brief
    ///   Bind shader resource view to the given shader register.
    ///
    /// @param shaderSpace          Shader space to be bind to.
    /// @param shaderRegister       Shader register to be bind to.
    /// @param shaderResourceView   Shader resource view CPU handle to be bind to the given shader register.
    YAGE_API auto BindShaderResourceView(uint32_t            shaderSpace,
                                         uint32_t            shaderRegister,
                                         CpuDescriptorHandle shaderResourceView) noexcept -> void;

    /// @brief
    ///   Create a new shader resource view for the given shader register.
    ///
    /// @param shaderSpace      Shader space to be bind to.
    /// @param shaderRegister   Shader register to be bind to.
    /// @param resource         Resource to create the new shader resource view.
    YAGE_API auto
    BindShaderResourceView(uint32_t shaderSpace, uint32_t shaderRegister, ID3D12Resource *resource) noexcept -> void;

    /// @brief
    ///   Create a new shader resource view for the given shader register.
    ///
    /// @param shaderSpace      Shader space to be bind to.
    /// @param shaderRegister   Shader register to be bind to.
    /// @param resource         Resource to create the new shader resource view.
    /// @param desc             Describes how to create the new shader resource view.
    YAGE_API auto BindShaderResourceView(uint32_t                               shaderSpace,
                                         uint32_t                               shaderRegister,
                                         ID3D12Resource                        *resource,
                                         const D3D12_SHADER_RESOURCE_VIEW_DESC &desc) noexcept -> void;

    /// @brief
    ///   Bind unordered access view to the given shader register.
    ///
    /// @param shaderSpace          Shader space to be bind to.
    /// @param shaderRegister       Shader register to be bind to.
    /// @param unorderedAccessView  Unordered access view CPU handle to be bind to the given shader register.
    YAGE_API auto BindUnorderedAccessView(uint32_t            shaderSpace,
                                          uint32_t            shaderRegister,
                                          CpuDescriptorHandle unorderedAccessView) noexcept -> void;

    /// @brief
    ///   Create a new unordered access view for the given shader register.
    ///
    /// @param shaderSpace      Shader space to be bind to.
    /// @param shaderRegister   Shader register to be bind to.
    /// @param resource         Resource to create the new unordered access view.
    /// @param desc             Describes how to create the new unordered access view.
    YAGE_API auto BindUnorderedAccessView(uint32_t                                shaderSpace,
                                          uint32_t                                shaderRegister,
                                          ID3D12Resource                         *resource,
                                          const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc) noexcept -> void;

    /// @brief
    ///   Bind sampler to the given shader register.
    ///
    /// @param shaderSpace      Shader space to be bind to.
    /// @param shaderRegister   Shader register to be bind to.
    /// @param sampler          Sampler CPU handle to be bind to the given shader register.
    YAGE_API auto BindSampler(uint32_t shaderSpace, uint32_t shaderRegister, CpuDescriptorHandle sampler) noexcept
        -> void;

    /// @brief
    ///   Create a new sampler for the given shader register.
    ///
    /// @param shaderSpace      Shader space to be bind to.
    /// @param shaderRegister   Shader register to be bind to.
    /// @param desc             Describes how to create the new sampler.
    YAGE_API auto BindSampler(uint32_t shaderSpace, uint32_t shaderRegister, const D3D12_SAMPLER_DESC &desc) noexcept
        -> void;

private:
    /// @brief  Render device that is used to create descriptor heaps.
    RenderDevice &renderDevice;

    /// @brief  D3D12 device that is used to copy descriptors.
    ID3D12Device *const device;

    /// @brief  Current root signature.
    RootSignature *rootSignature;

    /// @brief  Increment size of CBV/SRV/UAV.
    const uint32_t constantBufferViewSize;

    /// @brief  Increment size of sampler view.
    const uint32_t samplerViewSize;

    /// @brief  Shader visible CBV/SRV/UAV descriptor heap.
    ID3D12DescriptorHeap *descriptorHeap;

    /// @brief  Descriptor handle to the start of the descriptor heap.
    DescriptorHandle descriptorHeapStart;

    /// @brief  Retired shader visible CBV/SRV/UAV descriptor heaps.
    std::vector<ID3D12DescriptorHeap *> retiredDescriptorHeaps;

    /// @brief  Shader visible sampler descriptor heap.
    ID3D12DescriptorHeap *samplerHeap;

    /// @brief  Descriptor handle to the start of the sampler heap.
    DescriptorHandle samplerHeapStart;

    /// @brief  Retired shader visible sampler descriptor heaps.
    std::vector<ID3D12DescriptorHeap *> retiredSamplerHeaps;

    /// @brief  Number of free descriptors in current descriptor heap.
    uint32_t freeDescriptorCount;

    /// @brief  Number of free samplers in current sampler heap.
    uint32_t freeSamplerCount;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_DYNAMIC_DESCRIPTOR_HEAP_H
