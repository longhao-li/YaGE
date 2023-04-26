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

    /// @brief
    ///   Get singleton instance of this class.
    ///
    /// @param heapType Type of descriptor heap allocator to get.
    YAGE_NODISCARD static auto Singleton(D3D12_DESCRIPTOR_HEAP_TYPE heapType) -> DescriptorHeapAllocator &;

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

YAGE_NODISCARD auto DescriptorHeapAllocator::Singleton(D3D12_DESCRIPTOR_HEAP_TYPE heapType)
    -> DescriptorHeapAllocator & {
    if (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
        static DescriptorHeapAllocator instance(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        return instance;
    } else {
        static DescriptorHeapAllocator instance(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        return instance;
    }
}

} // namespace

YaGE::DynamicDescriptorHeap::DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE descriptorType)
    : device(RenderDevice::Singleton().Device()),
      descriptorType(descriptorType),
      descriptorSize(device->GetDescriptorHandleIncrementSize(descriptorType)),
      graphicsRootSignature(nullptr),
      computeRootSignature(nullptr),
      currentHeap(nullptr),
      currentHandle(),
      freeDescriptorCount(),
      retiredHeaps(),
      graphicsCachedParameters(),
      computeCachedParameters(),
      graphicsTableCache(),
      computeTableCache() {}

YaGE::DynamicDescriptorHeap::~DynamicDescriptorHeap() noexcept {
    if (currentHeap != nullptr) {
        retiredHeaps.push_back(currentHeap);
        currentHeap = nullptr;
    }

    const uint64_t syncPoint = RenderDevice::Singleton().AcquireSyncPoint();
    DescriptorHeapAllocator::Singleton(descriptorType).Free(syncPoint, retiredHeaps);
    retiredHeaps.clear();
}

auto YaGE::DynamicDescriptorHeap::CleanUp(uint64_t syncPoint) noexcept -> void {
    if (retiredHeaps.empty())
        return;

    DescriptorHeapAllocator::Singleton(descriptorType).Free(syncPoint, retiredHeaps);
    retiredHeaps.clear();

    graphicsRootSignature = nullptr;
    computeRootSignature  = nullptr;

    graphicsCachedParameters.clear();
    computeCachedParameters.clear();

    memset(graphicsTableCache, 0, sizeof(graphicsTableCache));
    memset(computeTableCache, 0, sizeof(computeTableCache));
}

auto YaGE::DynamicDescriptorHeap::ParseGraphicsRootSignature(RootSignature &rootSig) noexcept -> void {
    graphicsRootSignature = &rootSig;

    const uint32_t paramCount = (descriptorType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? rootSig.TableSamplerCount()
                                                                                      : rootSig.TableDescriptorCount());

    graphicsCachedParameters.resize(paramCount);
    memset(graphicsCachedParameters.data(), 0, paramCount * sizeof(CachedParameter));

    uint32_t offset = 0;
    if (descriptorType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
        for (uint32_t i = 0; i < 64; ++i) {
            uint32_t tableSize                   = rootSig.SamplerTableSize(i);
            graphicsTableCache[i].parameterCount = tableSize;
            graphicsTableCache[i].parameters     = graphicsCachedParameters.data() + offset;
            offset += tableSize;
        }
    } else {
        for (uint32_t i = 0; i < 64; ++i) {
            uint32_t tableSize                   = rootSig.NonSamplerDescriptorTableSize(i);
            graphicsTableCache[i].parameterCount = tableSize;
            graphicsTableCache[i].parameters     = graphicsCachedParameters.data() + offset;
            offset += tableSize;
        }
    }
}

auto YaGE::DynamicDescriptorHeap::BindGraphicsDescriptor(uint32_t            paramIndex,
                                                         uint32_t            offset,
                                                         CpuDescriptorHandle descriptor) noexcept -> void {
    assert(paramIndex < 64);

    DescriptorTableCache &tableCache = graphicsTableCache[paramIndex];
    if (offset >= tableCache.parameterCount)
        return; // No such descriptor.

    CachedParameter &param = tableCache.parameters[offset];

    param.parameterType              = ParameterType::DescriptorHandle;
    param.parameter.descriptorHandle = descriptor;
}

auto YaGE::DynamicDescriptorHeap::BindGraphicsDescriptor(uint32_t                               paramIndex,
                                                         uint32_t                               offset,
                                                         const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) noexcept -> void {

    assert(paramIndex < 64);

    DescriptorTableCache &tableCache = graphicsTableCache[paramIndex];
    if (offset >= tableCache.parameterCount)
        return; // No such descriptor.

    CachedParameter &param = tableCache.parameters[offset];

    param.parameterType                = ParameterType::ConstantBufferView;
    param.parameter.constantBufferView = desc;
}

auto YaGE::DynamicDescriptorHeap::ParseComputeRootSignature(RootSignature &rootSig) noexcept -> void {
    computeRootSignature = &rootSig;

    const uint32_t paramCount = (descriptorType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? rootSig.TableSamplerCount()
                                                                                      : rootSig.TableDescriptorCount());

    computeCachedParameters.resize(paramCount);
    memset(computeCachedParameters.data(), 0, paramCount * sizeof(CachedParameter));

    uint32_t offset = 0;
    if (descriptorType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
        for (uint32_t i = 0; i < 64; ++i) {
            uint32_t tableSize                  = rootSig.SamplerTableSize(i);
            computeTableCache[i].parameterCount = tableSize;
            computeTableCache[i].parameters     = computeCachedParameters.data() + offset;
            offset += tableSize;
        }
    } else {
        for (uint32_t i = 0; i < 64; ++i) {
            uint32_t tableSize                  = rootSig.NonSamplerDescriptorTableSize(i);
            computeTableCache[i].parameterCount = tableSize;
            computeTableCache[i].parameters     = computeCachedParameters.data() + offset;
            offset += tableSize;
        }
    }
}

auto YaGE::DynamicDescriptorHeap::BindComputeDescriptor(uint32_t            paramIndex,
                                                        uint32_t            offset,
                                                        CpuDescriptorHandle descriptor) noexcept -> void {
    assert(paramIndex < 64);

    DescriptorTableCache &tableCache = computeTableCache[paramIndex];
    if (offset >= tableCache.parameterCount)
        return; // No such descriptor.

    CachedParameter &param = tableCache.parameters[offset];

    param.parameterType              = ParameterType::DescriptorHandle;
    param.parameter.descriptorHandle = descriptor;
}

auto YaGE::DynamicDescriptorHeap::BindComputeDescriptor(uint32_t                               paramIndex,
                                                        uint32_t                               offset,
                                                        const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) noexcept -> void {
    assert(paramIndex < 64);

    DescriptorTableCache &tableCache = computeTableCache[paramIndex];
    if (offset >= tableCache.parameterCount)
        return; // No such descriptor.

    CachedParameter &param = tableCache.parameters[offset];

    param.parameterType                = ParameterType::ConstantBufferView;
    param.parameter.constantBufferView = desc;
}

auto YaGE::DynamicDescriptorHeap::Commit(ID3D12GraphicsCommandList *cmdList) noexcept -> void {
    assert(graphicsCachedParameters.size() + computeCachedParameters.size() < 1024);

    // Require new descriptor heap if no enough space.
    const uint32_t requiredCount =
        static_cast<uint32_t>(graphicsCachedParameters.size() + computeCachedParameters.size());

    if (requiredCount > freeDescriptorCount) {
        if (currentHeap != nullptr)
            retiredHeaps.push_back(currentHeap);

        auto &allocator     = DescriptorHeapAllocator::Singleton(descriptorType);
        currentHeap         = allocator.Allocate();
        currentHandle       = DescriptorHandle(currentHeap->GetCPUDescriptorHandleForHeapStart(),
                                               currentHeap->GetGPUDescriptorHandleForHeapStart());
        freeDescriptorCount = NUM_DESCRIPTORS_PER_HEAP;
    }

    if (requiredCount != 0)
        cmdList->SetDescriptorHeaps(1, &currentHeap);

    // Copy graphics descriptors.
    if (graphicsRootSignature != nullptr) {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> copySrc;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> copyDest;
        std::vector<UINT>                        copyCount;

        copySrc.reserve(graphicsCachedParameters.size());
        copyDest.reserve(graphicsCachedParameters.size());
        copyCount.reserve(graphicsCachedParameters.size());

        for (uint32_t i = 0; i < 64; ++i) {
            DescriptorTableCache &tableCache = graphicsTableCache[i];
            if (tableCache.parameterCount != 0)
                cmdList->SetGraphicsRootDescriptorTable(i, currentHandle);

            const CachedParameter *paramStart = tableCache.parameters;
            const CachedParameter *paramEnd   = paramStart + tableCache.parameterCount;

            for (const CachedParameter *param = paramStart; param != paramEnd; ++param) {
                switch (param->parameterType) {
                case ParameterType::DescriptorHandle:
                    copySrc.push_back(param->parameter.descriptorHandle);
                    copyDest.push_back(currentHandle);
                    copyCount.push_back(1);
                    break;

                case ParameterType::ConstantBufferView:
                    device->CreateConstantBufferView(&(param->parameter.constantBufferView), currentHandle);
                    break;

                default:
                    break;
                }

                currentHandle += descriptorSize;
                freeDescriptorCount -= 1;
            }
        }

        if (!copySrc.empty())
            device->CopyDescriptors(static_cast<UINT>(copyDest.size()), copyDest.data(), copyCount.data(),
                                    static_cast<UINT>(copySrc.size()), copySrc.data(), copyCount.data(),
                                    descriptorType);
    }

    // Copy compute descriptors.
    if (computeRootSignature != nullptr) {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> copySrc;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> copyDest;
        std::vector<UINT>                        copyCount;

        copySrc.reserve(computeCachedParameters.size());
        copyDest.reserve(computeCachedParameters.size());
        copyCount.reserve(computeCachedParameters.size());

        for (uint32_t i = 0; i < 64; ++i) {
            DescriptorTableCache &tableCache = computeTableCache[i];
            if (tableCache.parameterCount != 0)
                cmdList->SetComputeRootDescriptorTable(i, currentHandle);

            const CachedParameter *paramStart = tableCache.parameters;
            const CachedParameter *paramEnd   = paramStart + tableCache.parameterCount;

            for (const CachedParameter *param = paramStart; param != paramEnd; ++param) {
                switch (param->parameterType) {
                case ParameterType::DescriptorHandle:
                    copySrc.push_back(param->parameter.descriptorHandle);
                    copyDest.push_back(currentHandle);
                    copyCount.push_back(1);
                    break;

                case ParameterType::ConstantBufferView:
                    device->CreateConstantBufferView(&(param->parameter.constantBufferView), currentHandle);
                    break;

                default:
                    break;
                }

                currentHandle += descriptorSize;
                freeDescriptorCount -= 1;
            }
        }

        if (!copySrc.empty())
            device->CopyDescriptors(static_cast<UINT>(copyDest.size()), copyDest.data(), copyCount.data(),
                                    static_cast<UINT>(copySrc.size()), copySrc.data(), copyCount.data(),
                                    descriptorType);
    }
}
