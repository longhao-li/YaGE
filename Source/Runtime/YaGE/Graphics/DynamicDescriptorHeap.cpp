#include "DynamicDescriptorHeap.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

#include <cassert>
#include <mutex>
#include <queue>
#include <stack>

using namespace YaGE;
using Microsoft::WRL::ComPtr;

namespace {

static constexpr const uint32_t NUM_DESCRIPTORS_PER_HEAP = 1024;

class DescriptorHeapAllocator {
public:
    /// @brief
    ///   Create a new descriptor heap allocator.
    ///
    /// @param heapType Descriptor heap type.
    DescriptorHeapAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heapType);

    /// @brief
    ///   Copy constructor is disabled.
    DescriptorHeapAllocator(const DescriptorHeapAllocator &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const DescriptorHeapAllocator &) = delete;

    /// @brief
    ///   Destroy this descriptor heap allocator.
    ~DescriptorHeapAllocator() noexcept;

    /// @brief
    ///   Allocate a free descriptor heap.
    ///
    /// @return ID3D12DescriptorHeap *
    ///   Return a free descriptor heap.
    /// @throw RenderAPIException
    ///   Thrown if failed to create new descriptor heap.
    YAGE_NODISCARD auto Allocate() -> ID3D12DescriptorHeap *;

    /// @brief
    ///   Free descriptor heaps.
    ///
    /// @param syncPoint  Sync point that is used to determine when the freed descriptor heaps could be reused.
    /// @param heaps      Descriptor heaps to be freed.
    auto Free(uint64_t syncPoint, const std::vector<ID3D12DescriptorHeap *> &heaps) noexcept -> void;

private:
    /// @brief  Render device that is used to create descriptor heaps.
    RenderDevice &renderDevice;

    /// @brief  Descriptor heap type.
    const D3D12_DESCRIPTOR_HEAP_TYPE heapType;

    /// @brief  Allocated descriptor heaps.
    std::stack<ComPtr<ID3D12DescriptorHeap>> heapPool;

    /// @brief  Mutex that is used to protect heap pool.
    mutable std::mutex heapPoolMutex;

    /// @brief  Retired descriptor heap queue.
    std::queue<std::pair<uint64_t, ID3D12DescriptorHeap *>> retiredHeaps;

    /// @brief  Mutex that is used to protect retired heap queue.
    mutable std::mutex retiredHeapsMutex;
};

DescriptorHeapAllocator::DescriptorHeapAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heapType)
    : renderDevice(RenderDevice::Singleton()),
      heapType(heapType),
      heapPool(),
      heapPoolMutex(),
      retiredHeaps(),
      retiredHeapsMutex() {}

DescriptorHeapAllocator::~DescriptorHeapAllocator() noexcept {}

YAGE_NODISCARD auto DescriptorHeapAllocator::Allocate() -> ID3D12DescriptorHeap * {
    { // Try to acquire one from retired queue.
        std::lock_guard<std::mutex> lock(retiredHeapsMutex);
        if (!retiredHeaps.empty()) {
            auto &front = retiredHeaps.front();
            if (renderDevice.IsSyncPointReached(front.first)) {
                ID3D12DescriptorHeap *heap = front.second;
                retiredHeaps.pop();
                return heap;
            }
        }
    }

    // Try to create a new one.
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    const D3D12_DESCRIPTOR_HEAP_DESC desc{
        /* Type           = */ heapType,
        /* NumDescriptors = */ NUM_DESCRIPTORS_PER_HEAP,
        /* Flags          = */ D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        /* NodeMask       = */ 0,
    };

    HRESULT hr = renderDevice.Device()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create dynamic descriptor heap.");

    ID3D12DescriptorHeap *result = descriptorHeap.Get();

    // Add to heap pool.
    std::lock_guard<std::mutex> lock(heapPoolMutex);
    heapPool.push(std::move(descriptorHeap));
    return result;
}

auto DescriptorHeapAllocator::Free(uint64_t syncPoint, const std::vector<ID3D12DescriptorHeap *> &heaps) noexcept
    -> void {
    std::lock_guard<std::mutex> lock(retiredHeapsMutex);
    for (auto heap : heaps)
        retiredHeaps.emplace(syncPoint, heap);
}

YAGE_NODISCARD auto DefaultDescriptorHeapAllocator() -> DescriptorHeapAllocator & {
    static DescriptorHeapAllocator instance(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    return instance;
}

YAGE_NODISCARD auto SamplerDescriptorHeapAllocator() -> DescriptorHeapAllocator & {
    static DescriptorHeapAllocator instance(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    return instance;
}

} // namespace

YaGE::DynamicDescriptorHeap::DynamicDescriptorHeap()
    : renderDevice(RenderDevice::Singleton()),
      device(renderDevice.Device()),
      rootSignature(nullptr),
      constantBufferViewSize(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)),
      samplerViewSize(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)),
      descriptorHeap(),
      descriptorHeapStart(),
      retiredDescriptorHeaps(),
      samplerHeap(),
      samplerHeapStart(),
      retiredSamplerHeaps() {}

YaGE::DynamicDescriptorHeap::~DynamicDescriptorHeap() noexcept {
    uint64_t syncPoint = renderDevice.AcquireSyncPoint();

    if (descriptorHeap != nullptr)
        retiredDescriptorHeaps.push_back(descriptorHeap);

    if (!retiredDescriptorHeaps.empty()) {
        DefaultDescriptorHeapAllocator().Free(syncPoint, retiredDescriptorHeaps);
        retiredDescriptorHeaps.clear();
    }

    if (samplerHeap != nullptr)
        retiredSamplerHeaps.push_back(samplerHeap);

    if (!retiredSamplerHeaps.empty()) {
        SamplerDescriptorHeapAllocator().Free(syncPoint, retiredSamplerHeaps);
        retiredSamplerHeaps.clear();
    }
}

auto YaGE::DynamicDescriptorHeap::ParseRootSignature(RootSignature &rootSig) -> void {
    rootSignature = &rootSig;

    // Allocate a new descriptor heap if necessary.
    if (freeDescriptorCount < rootSig.DescriptorCount()) {
        if (descriptorHeap != nullptr)
            retiredDescriptorHeaps.push_back(descriptorHeap);

        descriptorHeap      = DefaultDescriptorHeapAllocator().Allocate();
        descriptorHeapStart = DescriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                               descriptorHeap->GetGPUDescriptorHandleForHeapStart());
        freeDescriptorCount = NUM_DESCRIPTORS_PER_HEAP;
    } else {
        const uint32_t offset = NUM_DESCRIPTORS_PER_HEAP - freeDescriptorCount;
        descriptorHeapStart   = DescriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                                 descriptorHeap->GetGPUDescriptorHandleForHeapStart());
        descriptorHeapStart += (offset * constantBufferViewSize);
    }

    freeDescriptorCount -= rootSig.DescriptorCount();

    // Allocate a new sampler descriptor heap if necessary.
    if (freeSamplerCount < rootSig.SamplerCount()) {
        if (samplerHeap != nullptr)
            retiredSamplerHeaps.push_back(samplerHeap);

        samplerHeap      = SamplerDescriptorHeapAllocator().Allocate();
        samplerHeapStart = DescriptorHandle(samplerHeap->GetCPUDescriptorHandleForHeapStart(),
                                            samplerHeap->GetGPUDescriptorHandleForHeapStart());
        freeSamplerCount = NUM_DESCRIPTORS_PER_HEAP;
    } else {
        const uint32_t offset = NUM_DESCRIPTORS_PER_HEAP - freeSamplerCount;
        samplerHeapStart      = DescriptorHandle(samplerHeap->GetCPUDescriptorHandleForHeapStart(),
                                                 samplerHeap->GetGPUDescriptorHandleForHeapStart());
        samplerHeapStart += (offset * samplerViewSize);
    }

    freeSamplerCount -= rootSig.SamplerCount();
}

auto YaGE::DynamicDescriptorHeap::Reset(uint64_t syncPoint) noexcept -> void {
    if (!retiredDescriptorHeaps.empty()) {
        DefaultDescriptorHeapAllocator().Free(syncPoint, retiredDescriptorHeaps);
        retiredDescriptorHeaps.clear();
    }

    if (!retiredSamplerHeaps.empty()) {
        SamplerDescriptorHeapAllocator().Free(syncPoint, retiredSamplerHeaps);
        retiredSamplerHeaps.clear();
    }
}

auto YaGE::DynamicDescriptorHeap::BindGraphics(ID3D12GraphicsCommandList *cmdList) noexcept -> void {
    if (rootSignature->DescriptorCount() != 0) {
        cmdList->SetDescriptorHeaps(1, &descriptorHeap);
        cmdList->SetGraphicsRootDescriptorTable(0, descriptorHeapStart);
    }

    if (rootSignature->SamplerCount() != 0) {
        const uint32_t paramIndex = rootSignature->DescriptorCount() == 0 ? 0 : 1;
        cmdList->SetDescriptorHeaps(1, &samplerHeap);
        cmdList->SetGraphicsRootDescriptorTable(paramIndex, samplerHeapStart);
    }
}

auto YaGE::DynamicDescriptorHeap::BindCompute(ID3D12GraphicsCommandList *cmdList) noexcept -> void {
    if (rootSignature->DescriptorCount() != 0) {
        cmdList->SetDescriptorHeaps(1, &descriptorHeap);
        cmdList->SetComputeRootDescriptorTable(0, descriptorHeapStart);
    }

    if (rootSignature->SamplerCount() != 0) {
        const uint32_t paramIndex = rootSignature->DescriptorCount() == 0 ? 0 : 1;
        cmdList->SetDescriptorHeaps(1, &samplerHeap);
        cmdList->SetComputeRootDescriptorTable(paramIndex, samplerHeapStart);
    }
}

auto YaGE::DynamicDescriptorHeap::BindConstantBufferView(uint32_t            shaderSpace,
                                                         uint32_t            shaderRegister,
                                                         CpuDescriptorHandle constantBufferView) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->constantBufferViewOffset[shaderSpace];
    DescriptorHandle handle = descriptorHeapStart + ((offset + shaderRegister) * constantBufferViewSize);
    device->CopyDescriptorsSimple(1, handle, constantBufferView, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

auto YaGE::DynamicDescriptorHeap::BindConstantBufferView(uint32_t shaderSpace,
                                                         uint32_t shaderRegister,
                                                         uint64_t gpuAddress,
                                                         uint32_t size) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->constantBufferViewOffset[shaderSpace];
    DescriptorHandle handle = descriptorHeapStart + ((offset + shaderRegister) * constantBufferViewSize);

    const D3D12_CONSTANT_BUFFER_VIEW_DESC desc{
        /* BufferLocation = */ gpuAddress,
        /* SizeInBytes    = */ size,
    };

    device->CreateConstantBufferView(&desc, handle);
}

auto YaGE::DynamicDescriptorHeap::BindShaderResourceView(uint32_t            shaderSpace,
                                                         uint32_t            shaderRegister,
                                                         CpuDescriptorHandle shaderResourceView) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->shaderResourceViewOffset[shaderSpace];
    DescriptorHandle handle = descriptorHeapStart + ((offset + shaderRegister) * constantBufferViewSize);
    device->CopyDescriptorsSimple(1, handle, shaderResourceView, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

auto YaGE::DynamicDescriptorHeap::BindShaderResourceView(uint32_t        shaderSpace,
                                                         uint32_t        shaderRegister,
                                                         ID3D12Resource *resource) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->shaderResourceViewOffset[shaderSpace];
    DescriptorHandle handle = descriptorHeapStart + ((offset + shaderRegister) * constantBufferViewSize);
    device->CreateShaderResourceView(resource, nullptr, handle);
}

auto YaGE::DynamicDescriptorHeap::BindShaderResourceView(uint32_t                               shaderSpace,
                                                         uint32_t                               shaderRegister,
                                                         ID3D12Resource                        *resource,
                                                         const D3D12_SHADER_RESOURCE_VIEW_DESC &desc) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->shaderResourceViewOffset[shaderSpace];
    DescriptorHandle handle = descriptorHeapStart + ((offset + shaderRegister) * constantBufferViewSize);
    device->CreateShaderResourceView(resource, &desc, handle);
}

auto YaGE::DynamicDescriptorHeap::BindUnorderedAccessView(uint32_t            shaderSpace,
                                                          uint32_t            shaderRegister,
                                                          CpuDescriptorHandle unorderedAccessView) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->unorderedAccessViewOffset[shaderSpace];
    DescriptorHandle handle = descriptorHeapStart + ((offset + shaderRegister) * constantBufferViewSize);
    device->CopyDescriptorsSimple(1, handle, unorderedAccessView, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

auto YaGE::DynamicDescriptorHeap::BindUnorderedAccessView(uint32_t                                shaderSpace,
                                                          uint32_t                                shaderRegister,
                                                          ID3D12Resource                         *resource,
                                                          const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc) noexcept
    -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->unorderedAccessViewOffset[shaderSpace];
    DescriptorHandle handle = descriptorHeapStart + ((offset + shaderRegister) * constantBufferViewSize);
    device->CreateUnorderedAccessView(resource, nullptr, &desc, handle);
}

auto YaGE::DynamicDescriptorHeap::BindSampler(uint32_t            shaderSpace,
                                              uint32_t            shaderRegister,
                                              CpuDescriptorHandle sampler) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->samplerViewOffset[shaderSpace];
    DescriptorHandle handle = samplerHeapStart + ((offset + shaderRegister) * samplerViewSize);
    device->CopyDescriptorsSimple(1, handle, sampler, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

auto YaGE::DynamicDescriptorHeap::BindSampler(uint32_t                  shaderSpace,
                                              uint32_t                  shaderRegister,
                                              const D3D12_SAMPLER_DESC &desc) noexcept -> void {
    assert(shaderSpace < 16);
    const uint32_t   offset = rootSignature->samplerViewOffset[shaderSpace];
    DescriptorHandle handle = samplerHeapStart + ((offset + shaderRegister) * samplerViewSize);
    device->CreateSampler(&desc, handle);
}
