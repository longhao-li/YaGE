#include "Texture.h"
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

YaGE::Texture::Texture() noexcept : PixelBuffer(), isCubeTexture(), srv() {}

YaGE::Texture::Texture(
    uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format, uint32_t mipmapLevels, bool isCubeTexture)
    : PixelBuffer(), isCubeTexture((arraySize % 6 == 0) && isCubeTexture), srv() {
    // Clamp mipmap levels.
    const uint32_t maxMipLevels = MaxMipLevels(width | height);
    if (mipmapLevels == 0 || mipmapLevels > maxMipLevels)
        mipmapLevels = maxMipLevels;

    this->width       = width;
    this->height      = height;
    this->arraySize   = arraySize;
    this->sampleCount = 1;
    this->mipLevels   = mipmapLevels;
    this->pixelFormat = format;

    // Create D3D12 resources.
    RenderDevice &device = RenderDevice::Singleton();

    { // Create ID3D12Resource
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
            /* DepthOrArraySize = */ static_cast<UINT16>(arraySize),
            /* MipLevels        = */ static_cast<UINT16>(mipmapLevels),
            /* Format           = */ format,
            /* SampleDesc       = */
            {
                /* Count   = */ 1,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_UNKNOWN,
            /* Flags  = */ D3D12_RESOURCE_FLAG_NONE,
        };

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create ID3D12Resource for Texture.");
    }

    // Create shader resource view.
    if (this->isCubeTexture) {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc;
        desc.Format                  = format;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        if (arraySize > 6) {
            desc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
            desc.TextureCubeArray.MostDetailedMip     = 0;
            desc.TextureCubeArray.MipLevels           = mipmapLevels;
            desc.TextureCubeArray.First2DArrayFace    = 0;
            desc.TextureCubeArray.NumCubes            = arraySize / 6;
            desc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
        } else {
            desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
            desc.TextureCube.MostDetailedMip     = 0;
            desc.TextureCube.MipLevels           = mipmapLevels;
            desc.TextureCube.ResourceMinLODClamp = 0.0f;
        }

        srv.Create(resource.Get(), desc);
    } else {
        srv.Create(resource.Get());
    }
}

YaGE::Texture::Texture(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipmapLevels)
    : PixelBuffer(), isCubeTexture(), srv() {
    // Clamp mipmap levels.
    const uint32_t maxMipLevels = MaxMipLevels(width | height);
    if (mipmapLevels == 0 || mipmapLevels > maxMipLevels)
        mipmapLevels = maxMipLevels;

    this->width       = width;
    this->height      = height;
    this->arraySize   = 1;
    this->sampleCount = 1;
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

        const D3D12_RESOURCE_DESC desc{
            /* Dimension        = */ D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            /* Alignment        = */ 0,
            /* Width            = */ width,
            /* Height           = */ height,
            /* DepthOrArraySize = */ 1,
            /* MipLevels        = */ static_cast<UINT16>(mipmapLevels),
            /* Format           = */ format,
            /* SampleDesc       = */
            {
                /* Count   = */ 1,
                /* Quality = */ 0,
            },
            /* Layout = */ D3D12_TEXTURE_LAYOUT_UNKNOWN,
            /* Flags  = */ D3D12_RESOURCE_FLAG_NONE,
        };

        HRESULT hr = device.Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                              D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                              IID_PPV_ARGS(resource.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create ID3D12Resource for Texture.");
    }

    // Create shader resource view.
    srv.Create(resource.Get());
}

YaGE::Texture::Texture(Texture &&other) noexcept
    : PixelBuffer(std::move(other)), isCubeTexture(other.isCubeTexture), srv(std::move(other.srv)) {
    other.isCubeTexture = false;
}

auto YaGE::Texture::operator=(Texture &&other) noexcept -> Texture & {
    PixelBuffer::operator=(std::move(other));

    isCubeTexture = other.isCubeTexture;
    srv           = std::move(other.srv);

    other.isCubeTexture = false;

    return *this;
}

YaGE::Texture::~Texture() noexcept {}
