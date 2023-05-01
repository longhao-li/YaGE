#include "GpuBuffer.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

using namespace YaGE;

YaGE::GpuBuffer::GpuBuffer() noexcept : GpuResource(), bufferSize(), address() {}

YaGE::GpuBuffer::GpuBuffer(size_t size) : GpuResource(), bufferSize(), address() {
    // Align up.
    size       = ((size + 255) & ~size_t(255));
    bufferSize = size;

    // Create D3D12 resources.
    RenderDevice &device = RenderDevice::Singleton();

    { // Create ID3D12Resource.
        const D3D12_HEAP_PROPERTIES heapProps{
            /* Type                 = */ D3D12_HEAP_TYPE_DEFAULT,
            /* CPUPageProperty      = */ D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            /* MemoryPoolPreference = */ D3D12_MEMORY_POOL_UNKNOWN,
            /* CreationNodeMask     = */ 0,
            /* VisibleNodeMask      = */ 0,
        };

        const D3D12_RESOURCE_DESC desc{
            /* Dimension        = */ D3D12_RESOURCE_DIMENSION_BUFFER,
            /* Alignment        = */ 0,
            /* Width            = */ size,
            /* Height           = */ 1,
            /* DepthOrArraySize = */ 1,
            /* MipLevels        = */ 1,
            /* Format           = */ DXGI_FORMAT_UNKNOWN,
            /* SampleDesc       = */
            {
                /* Count   = */ 1,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            /* Flags  = */ D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
        };

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create ID3D12Resource for GpuBuffer.");

        this->address = resource->GetGPUVirtualAddress();
    }

    { // Create byte address unordered access view.
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
        desc.Format                      = DXGI_FORMAT_R32_TYPELESS;
        desc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement         = 0;
        desc.Buffer.NumElements          = static_cast<UINT>(size / 4);
        desc.Buffer.StructureByteStride  = 0;
        desc.Buffer.CounterOffsetInBytes = 0;
        desc.Buffer.Flags                = D3D12_BUFFER_UAV_FLAG_RAW;

        byteAddressUAV.Create(resource.Get(), nullptr, desc);
    }
}

YaGE::GpuBuffer::GpuBuffer(GpuBuffer &&other) noexcept
    : GpuResource(std::move(other)),
      bufferSize(other.bufferSize),
      address(other.address),
      byteAddressUAV(std::move(other.byteAddressUAV)) {
    other.bufferSize = 0;
    other.address    = 0;
}

auto YaGE::GpuBuffer::operator=(GpuBuffer &&other) noexcept -> GpuBuffer & {
    GpuResource::operator=(std::move(other));

    bufferSize     = other.bufferSize;
    address        = other.address;
    byteAddressUAV = std::move(other.byteAddressUAV);

    other.bufferSize = 0;
    other.address    = 0;

    return *this;
}

YaGE::GpuBuffer::~GpuBuffer() noexcept {}

YaGE::StructuredBuffer::StructuredBuffer() noexcept : GpuBuffer(), elementSize(), elementCount(), structedBufferUAV() {}

YaGE::StructuredBuffer::StructuredBuffer(uint32_t count, uint32_t size)
    : GpuBuffer(count * size_t(size)), elementSize(size), elementCount(count), structedBufferUAV() {
    // Create structured buffer unordered access view.
    D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
    desc.Format                      = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
    desc.Buffer.FirstElement         = 0;
    desc.Buffer.NumElements          = count;
    desc.Buffer.StructureByteStride  = size;
    desc.Buffer.CounterOffsetInBytes = 0;
    desc.Buffer.Flags                = D3D12_BUFFER_UAV_FLAG_NONE;

    structedBufferUAV.Create(resource.Get(), desc);
}

YaGE::StructuredBuffer::StructuredBuffer(StructuredBuffer &&other) noexcept
    : GpuBuffer(std::move(other)),
      elementSize(other.elementSize),
      elementCount(other.elementCount),
      structedBufferUAV(std::move(other.structedBufferUAV)) {
    other.elementSize  = 0;
    other.elementCount = 0;
}

auto YaGE::StructuredBuffer::operator=(StructuredBuffer &&other) noexcept -> StructuredBuffer & {
    GpuBuffer::operator=(std::move(other));

    elementSize       = other.elementSize;
    elementCount      = other.elementCount;
    structedBufferUAV = std::move(other.structedBufferUAV);

    other.elementSize  = 0;
    other.elementCount = 0;

    return *this;
}

YaGE::StructuredBuffer::~StructuredBuffer() noexcept {}
