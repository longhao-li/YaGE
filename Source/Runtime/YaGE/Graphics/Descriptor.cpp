#include "Descriptor.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

using namespace YaGE;

YaGE::CpuDescriptorAllocator::CpuDescriptorAllocator() noexcept
    : device(),
      heapType(),
      descriptorSize(),
      heapPool(),
      heapPoolMutex(),
      freeHandles(),
      currentHandle(),
      freeDescriptorCount() {}

YaGE::CpuDescriptorAllocator::~CpuDescriptorAllocator() noexcept {}

auto YaGE::CpuDescriptorAllocator::Initialize(ID3D12Device1 *dev, D3D12_DESCRIPTOR_HEAP_TYPE descriptorType) noexcept
    -> void {
    this->device         = dev;
    this->heapType       = descriptorType;
    this->descriptorSize = device->GetDescriptorHandleIncrementSize(descriptorType);
}

YAGE_NODISCARD auto YaGE::CpuDescriptorAllocator::Allocate() -> CpuDescriptorHandle {
    { // Try to get one from free handle queue.
        CpuDescriptorHandle handle;
        if (freeHandles.try_pop(handle))
            return handle;
    }

    std::lock_guard<std::mutex> lock(heapPoolMutex);

    // No more free descriptors. Create a new descriptor heap.
    if (freeDescriptorCount == 0) {
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> newHeap;

        // Use a small number of descriptors to avoid wasting too much memory.
        const D3D12_DESCRIPTOR_HEAP_DESC desc{
            /* Type           = */ heapType,
            /* NumDescriptors = */ 64,
            /* Flags          = */ D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
            /* NodeMask       = */ 0,
        };

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(newHeap.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create new descriptor heap.");

        currentHandle       = newHeap->GetCPUDescriptorHandleForHeapStart();
        freeDescriptorCount = desc.NumDescriptors;

        heapPool.push(std::move(newHeap));
    }

    CpuDescriptorHandle result = currentHandle;

    currentHandle += descriptorSize;
    freeDescriptorCount -= 1;

    return result;
}

auto YaGE::ConstantBufferView::operator=(ConstantBufferView &&other) noexcept -> ConstantBufferView & {
    RenderDevice &device = RenderDevice::Singleton();
    if (!handle.IsNull())
        device.FreeConstantBufferView(handle);

    handle       = other.handle;
    other.handle = CpuDescriptorHandle();

    return *this;
}

YaGE::ConstantBufferView::~ConstantBufferView() noexcept {
    if (!handle.IsNull())
        RenderDevice::Singleton().FreeConstantBufferView(handle);
}

auto YaGE::ConstantBufferView::Create(uint64_t gpuAddress, uint32_t size) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateConstantBufferView();

    const D3D12_CONSTANT_BUFFER_VIEW_DESC desc{
        /* BufferLocation = */ gpuAddress,
        /* SizeInBytes    = */ size,
    };

    device.Device()->CreateConstantBufferView(&desc, handle);
}

auto YaGE::ConstantBufferView::Create(const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateConstantBufferView();

    device.Device()->CreateConstantBufferView(&desc, handle);
}

auto YaGE::ShaderResourceView::operator=(ShaderResourceView &&other) noexcept -> ShaderResourceView & {
    RenderDevice &device = RenderDevice::Singleton();
    if (!handle.IsNull())
        device.FreeShaderResourceView(handle);

    handle       = other.handle;
    other.handle = CpuDescriptorHandle();

    return *this;
}

YaGE::ShaderResourceView::~ShaderResourceView() noexcept {
    if (!handle.IsNull())
        RenderDevice::Singleton().FreeShaderResourceView(handle);
}

auto YaGE::ShaderResourceView::Create(ID3D12Resource *resource) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateShaderResourceView();

    device.Device()->CreateShaderResourceView(resource, nullptr, handle);
}

auto YaGE::ShaderResourceView::Create(ID3D12Resource *resource, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateShaderResourceView();

    device.Device()->CreateShaderResourceView(resource, &desc, handle);
}

auto YaGE::UnorderedAccessView::operator=(UnorderedAccessView &&other) noexcept -> UnorderedAccessView & {
    RenderDevice &device = RenderDevice::Singleton();
    if (!handle.IsNull())
        device.FreeUnorderedAccessView(handle);

    handle       = other.handle;
    other.handle = CpuDescriptorHandle();

    return *this;
}

YaGE::UnorderedAccessView::~UnorderedAccessView() noexcept {
    if (!handle.IsNull())
        RenderDevice::Singleton().FreeUnorderedAccessView(handle);
}

auto YaGE::UnorderedAccessView::Create(ID3D12Resource *resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateUnorderedAccessView();

    device.Device()->CreateUnorderedAccessView(resource, nullptr, &desc, handle);
}

auto YaGE::UnorderedAccessView::Create(ID3D12Resource                         *resource,
                                       ID3D12Resource                         *counter,
                                       const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateUnorderedAccessView();

    device.Device()->CreateUnorderedAccessView(resource, counter, &desc, handle);
}

auto YaGE::SamplerView::operator=(SamplerView &&other) noexcept -> SamplerView & {
    RenderDevice &device = RenderDevice::Singleton();
    if (!handle.IsNull())
        device.FreeSamplerView(handle);

    handle       = other.handle;
    other.handle = CpuDescriptorHandle();

    return *this;
}

YaGE::SamplerView::~SamplerView() noexcept {
    if (!handle.IsNull())
        RenderDevice::Singleton().FreeSamplerView(handle);
}

auto YaGE::SamplerView::Create(const D3D12_SAMPLER_DESC &desc) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateSamplerView();

    device.Device()->CreateSampler(&desc, handle);
}

auto YaGE::RenderTargetView::operator=(RenderTargetView &&other) noexcept -> RenderTargetView & {
    RenderDevice &device = RenderDevice::Singleton();
    if (!handle.IsNull())
        device.FreeRenderTargetView(handle);

    handle       = other.handle;
    other.handle = CpuDescriptorHandle();

    return *this;
}

YaGE::RenderTargetView::~RenderTargetView() noexcept {
    if (!handle.IsNull())
        RenderDevice::Singleton().FreeRenderTargetView(handle);
}

auto YaGE::RenderTargetView::Create(ID3D12Resource *resource, const D3D12_RENDER_TARGET_VIEW_DESC &desc) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateRenderTargetView();

    device.Device()->CreateRenderTargetView(resource, &desc, handle);
}

auto YaGE::DepthStencilView::operator=(DepthStencilView &&other) noexcept -> DepthStencilView & {
    RenderDevice &device = RenderDevice::Singleton();
    if (!handle.IsNull())
        device.FreeDepthStencilView(handle);

    handle       = other.handle;
    other.handle = CpuDescriptorHandle();

    return *this;
}

YaGE::DepthStencilView::~DepthStencilView() noexcept {
    if (!handle.IsNull())
        RenderDevice::Singleton().FreeDepthStencilView(handle);
}

auto YaGE::DepthStencilView::Create(ID3D12Resource *resource, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc) -> void {
    RenderDevice &device = RenderDevice::Singleton();

    if (handle.IsNull())
        handle = device.AllocateDepthStencilView();

    device.Device()->CreateDepthStencilView(resource, &desc, handle);
}
