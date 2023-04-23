#include "ColorBuffer.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

using namespace YaGE;

YAGE_FORCEINLINE static auto MaxMipLevels(uint32_t width) noexcept -> uint32_t {
    uint32_t result = 0;
    while (width) {
        width >>= 1;
        result += 1;
    }

    return result;
}

YaGE::ColorBuffer::ColorBuffer() noexcept : PixelBuffer(), clearColor(), rtv(), srv(), uav() {}

YaGE::ColorBuffer::ColorBuffer(uint32_t    width,
                               uint32_t    height,
                               uint32_t    arraySize,
                               DXGI_FORMAT format,
                               uint32_t    mipmapLevels,
                               uint32_t    sampleCount)
    : PixelBuffer(), clearColor(), rtv(), srv(), uav() {
    // Clamp mipmap levels.
    const uint32_t maxMipLevels = MaxMipLevels(width | height);
    if (mipmapLevels == 0 || mipmapLevels > maxMipLevels)
        mipmapLevels = maxMipLevels;

    if (sampleCount == 0)
        sampleCount = 1;

    this->width       = width;
    this->height      = height;
    this->arraySize   = arraySize;
    this->sampleCount = sampleCount;
    this->mipLevels   = mipmapLevels;
    this->pixelFormat = format;

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

        // Enable unordered access if multi-sample is not enabled.
        D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        if (sampleCount == 1)
            resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        const D3D12_RESOURCE_DESC desc{
            /* Dimension        = */ D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            /* Alignment        = */ 0,
            /* Width            = */ width,
            /* Height           = */ height,
            /* DepthOrArraySize = */ static_cast<UINT16>(arraySize),
            /* MipLevels        = */ static_cast<UINT16>(mipmapLevels),
            /* Format           = */ format,
            /* SampleDesc       = */
            {
                /* Count   = */ sampleCount,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_UNKNOWN,
            /* Flags  = */ resourceFlags,
        };

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create ID3D12Resource for ColorBuffer.");
    }

    { // Create render target view.
        D3D12_RENDER_TARGET_VIEW_DESC desc;
        desc.Format = format;

        if (arraySize > 1 && sampleCount > 1) {
            desc.ViewDimension                    = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
            desc.Texture2DMSArray.FirstArraySlice = 0;
            desc.Texture2DMSArray.ArraySize       = arraySize;
        } else if (arraySize > 1) {
            desc.ViewDimension                  = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MipSlice        = 0;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.ArraySize       = arraySize;
            desc.Texture2DArray.PlaneSlice      = 0;
        } else if (sampleCount > 1) {
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        } else {
            desc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice   = 0;
            desc.Texture2D.PlaneSlice = 0;
        }

        rtv.Create(this->resource.Get(), desc);
    }

    { // Create shader resource view
        D3D12_SHADER_RESOURCE_VIEW_DESC desc;
        desc.Format                  = format;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (arraySize > 1 && sampleCount > 1) {
            desc.ViewDimension                    = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
            desc.Texture2DMSArray.FirstArraySlice = 0;
            desc.Texture2DMSArray.ArraySize       = arraySize;
        } else if (arraySize > 1) {
            desc.ViewDimension                      = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MostDetailedMip     = 0;
            desc.Texture2DArray.MipLevels           = mipmapLevels;
            desc.Texture2DArray.FirstArraySlice     = 0;
            desc.Texture2DArray.ArraySize           = arraySize;
            desc.Texture2DArray.PlaneSlice          = 0;
            desc.Texture2DArray.ResourceMinLODClamp = 0;
        } else if (sampleCount > 1) {
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        } else {
            desc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MostDetailedMip     = 0;
            desc.Texture2D.MipLevels           = mipmapLevels;
            desc.Texture2D.PlaneSlice          = 0;
            desc.Texture2D.ResourceMinLODClamp = 0;
        }

        srv.Create(this->resource.Get(), desc);
    }

    // Create unordered access view.
    if (sampleCount == 1) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
        desc.Format = format;

        if (arraySize > 1) {
            desc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MipSlice        = 0;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.ArraySize       = arraySize;
            desc.Texture2DArray.PlaneSlice      = 0;
        } else {
            desc.ViewDimension        = D3D12_UAV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice   = 0;
            desc.Texture2D.PlaneSlice = 0;
        }

        uav.Create(resource.Get(), desc);
    }
}

YaGE::ColorBuffer::ColorBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format)
    : PixelBuffer(), clearColor(), rtv(), srv(), uav() {
    this->width       = width;
    this->height      = height;
    this->arraySize   = 1;
    this->sampleCount = 1;
    this->mipLevels   = 1;
    this->pixelFormat = format;

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
            /* Dimension        = */ D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            /* Alignment        = */ 0,
            /* Width            = */ width,
            /* Height           = */ height,
            /* DepthOrArraySize = */ 1,
            /* MipLevels        = */ 1,
            /* Format           = */ format,
            /* SampleDesc       = */
            {
                /* Count   = */ 1,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_UNKNOWN,
            /* Flags  = */ D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
        };

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create ID3D12Resource for ColorBuffer.");
    }

    { // Create render target view.
        D3D12_RENDER_TARGET_VIEW_DESC desc;
        desc.Format               = format;
        desc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice   = 0;
        desc.Texture2D.PlaneSlice = 0;

        rtv.Create(this->resource.Get(), desc);
    }

    { // Create shader resource view
        D3D12_SHADER_RESOURCE_VIEW_DESC desc;
        desc.Format                        = format;
        desc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MostDetailedMip     = 0;
        desc.Texture2D.MipLevels           = 1;
        desc.Texture2D.PlaneSlice          = 0;
        desc.Texture2D.ResourceMinLODClamp = 0;

        srv.Create(this->resource.Get(), desc);
    }

    { // Create unordered access view.
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
        desc.Format               = format;
        desc.ViewDimension        = D3D12_UAV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice   = 0;
        desc.Texture2D.PlaneSlice = 0;

        uav.Create(resource.Get(), desc);
    }
}

YaGE::ColorBuffer::ColorBuffer(ColorBuffer &&other) noexcept
    : PixelBuffer(std::move(other)),
      clearColor(other.clearColor),
      rtv(std::move(other.rtv)),
      srv(std::move(other.srv)),
      uav(std::move(other.uav)) {
    other.clearColor = {};
}

auto YaGE::ColorBuffer::operator=(ColorBuffer &&other) noexcept -> ColorBuffer & {
    PixelBuffer::operator=(std::move(other));

    clearColor = other.clearColor;
    rtv        = std::move(other.rtv);
    srv        = std::move(other.srv);
    uav        = std::move(other.uav);

    other.clearColor = {};

    return *this;
}

YaGE::ColorBuffer::~ColorBuffer() noexcept {}

auto YaGE::ColorBuffer::ReleaseSwapChainResource() noexcept -> void { this->resource.Reset(); }

auto YaGE::ColorBuffer::ResetSwapChainResource(Microsoft::WRL::ComPtr<ID3D12Resource> buffer) -> void {
    const D3D12_RESOURCE_DESC desc = buffer->GetDesc();

    this->resource    = std::move(buffer);
    this->usageState  = D3D12_RESOURCE_STATE_PRESENT;
    this->width       = static_cast<uint32_t>(desc.Width);
    this->height      = static_cast<uint32_t>(desc.Height);
    this->arraySize   = desc.DepthOrArraySize;
    this->sampleCount = desc.SampleDesc.Count;
    this->mipLevels   = desc.MipLevels;
    this->pixelFormat = desc.Format;

    { // Create render target view.
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
        rtvDesc.Format               = pixelFormat;
        rtvDesc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice   = 0;
        rtvDesc.Texture2D.PlaneSlice = 0;

        rtv.Create(this->resource.Get(), rtvDesc);
    }
}
