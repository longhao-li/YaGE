#include "DepthBuffer.h"
#include "../Core/Exception.h"
#include "RenderDevice.h"

using namespace YaGE;

YAGE_NODISCARD YAGE_FORCEINLINE static auto GetDepthFormat(DXGI_FORMAT format) noexcept -> DXGI_FORMAT {
    switch (format) {
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;

    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        return DXGI_FORMAT_R16_UNORM;

    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

YaGE::DepthBuffer::DepthBuffer() noexcept
    : PixelBuffer(), clearDepth(1.0f), clearStencil(), dsv(), depthReadOnlyView(), srv() {}

YaGE::DepthBuffer::DepthBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t sampleCount)
    : PixelBuffer(), clearDepth(1.0f), clearStencil(), dsv(), depthReadOnlyView(), srv() {
    if (sampleCount == 0)
        sampleCount = 1;

    this->width       = width;
    this->height      = height;
    this->arraySize   = 1;
    this->sampleCount = sampleCount;
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
                /* Count   = */ sampleCount,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_UNKNOWN,
            /* Flags  = */ D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
        };

        D3D12_CLEAR_VALUE clearValue;
        clearValue.Format               = format;
        clearValue.DepthStencil.Depth   = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_COMMON, &clearValue,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create ID3D12Resource for DepthBuffer.");
    }

    { // Create depth stencil view.
        D3D12_DEPTH_STENCIL_VIEW_DESC desc;
        desc.Format = format;
        desc.Flags  = D3D12_DSV_FLAG_NONE;

        if (sampleCount > 1) {
            desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
        } else {
            desc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice = 0;
        }

        dsv.Create(this->resource.Get(), desc);
    }

    { // Create depth read-only view.
        D3D12_DEPTH_STENCIL_VIEW_DESC desc;
        desc.Format = format;
        desc.Flags  = D3D12_DSV_FLAG_READ_ONLY_DEPTH;

        if (sampleCount > 1) {
            desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
        } else {
            desc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice = 0;
        }

        depthReadOnlyView.Create(this->resource.Get(), desc);
    }

    { // Create shader resource view.
        D3D12_SHADER_RESOURCE_VIEW_DESC desc;
        desc.Format                  = GetDepthFormat(format);
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        if (sampleCount > 1) {
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        } else {
            desc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MostDetailedMip     = 0;
            desc.Texture2D.MipLevels           = 1;
            desc.Texture2D.PlaneSlice          = 0;
            desc.Texture2D.ResourceMinLODClamp = 0.0f;
        }

        srv.Create(this->resource.Get(), desc);
    }
}

YaGE::DepthBuffer::DepthBuffer(DepthBuffer &&other) noexcept
    : PixelBuffer(std::move(other)),
      clearDepth(other.clearDepth),
      clearStencil(other.clearStencil),
      dsv(std::move(other.dsv)),
      depthReadOnlyView(std::move(other.depthReadOnlyView)),
      srv(std::move(other.srv)) {
    other.clearDepth   = 1.0f;
    other.clearStencil = 0;
}

auto YaGE::DepthBuffer::operator=(DepthBuffer &&other) noexcept -> DepthBuffer & {
    PixelBuffer::operator=(std::move(other));

    clearDepth        = other.clearDepth;
    clearStencil      = other.clearStencil;
    dsv               = std::move(other.dsv);
    depthReadOnlyView = std::move(other.depthReadOnlyView);
    srv               = std::move(other.srv);

    other.clearDepth   = 1.0f;
    other.clearStencil = 0;

    return *this;
}

YaGE::DepthBuffer::~DepthBuffer() noexcept {}
