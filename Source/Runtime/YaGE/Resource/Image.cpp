#include "Image.h"
#include "../Core/Exception.h"

#include <wincodec.h>
#include <wrl/client.h>

#include <cassert>

using namespace YaGE;
using Microsoft::WRL::ComPtr;

namespace {

class WICImageFactory {
private:
    /// @brief
    ///   Create a WIC imaging factory.
    ///
    /// @throw SystemErrorException
    ///   Thrown if failed to initialize COM or failed to create WIC imaging factory.
    WICImageFactory() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr) && (hr != RPC_E_CHANGED_MODE))
            throw SystemErrorException(hr, u"Failed to initialize COM.");

        hr = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(factory.GetAddressOf()));
        if (FAILED(hr)) {
            CoUninitialize();
            throw SystemErrorException(hr, u"Failed to create WIC image factory.");
        }
    }

    /// @brief
    ///   Destroy this WIC imaging factory and uninitialize COM.
    ~WICImageFactory() noexcept {
        factory.Reset();
        CoUninitialize();
    }

public:
    YAGE_NODISCARD static auto Singleton() -> IWICImagingFactory * {
        static WICImageFactory instance;
        return instance.factory.Get();
    }

private:
    /// @brief  WIC image factory object.
    ComPtr<IWICImagingFactory> factory;
};

YAGE_NODISCARD static auto ToDXGIFormat(const WICPixelFormatGUID &guid) noexcept -> DXGI_FORMAT {
    constexpr const struct {
        const WICPixelFormatGUID &wic;
        DXGI_FORMAT               dxgiFormat;
    } convertMap[]{
        {GUID_WICPixelFormat128bppRGBAFloat, DXGI_FORMAT_R32G32B32A32_FLOAT},
        {GUID_WICPixelFormat64bppRGBAHalf, DXGI_FORMAT_R16G16B16A16_FLOAT},
        {GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM},
        {GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM},
        {GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM},
        {GUID_WICPixelFormat32bppBGR, DXGI_FORMAT_B8G8R8X8_UNORM},
        {GUID_WICPixelFormat32bppRGBA1010102XR, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM},
        {GUID_WICPixelFormat32bppRGBA1010102, DXGI_FORMAT_R10G10B10A2_UNORM},
        {GUID_WICPixelFormat16bppBGRA5551, DXGI_FORMAT_B5G5R5A1_UNORM},
        {GUID_WICPixelFormat16bppBGR565, DXGI_FORMAT_B5G6R5_UNORM},
        {GUID_WICPixelFormat32bppGrayFloat, DXGI_FORMAT_R32_FLOAT},
        {GUID_WICPixelFormat16bppGrayHalf, DXGI_FORMAT_R16_FLOAT},
        {GUID_WICPixelFormat16bppGray, DXGI_FORMAT_R16_UNORM},
        {GUID_WICPixelFormat8bppGray, DXGI_FORMAT_R8_UNORM},
        {GUID_WICPixelFormat8bppAlpha, DXGI_FORMAT_A8_UNORM},
        {GUID_WICPixelFormatBlackWhite, DXGI_FORMAT_R1_UNORM},
    };

    for (const auto &c : convertMap) {
        if (c.wic == guid)
            return c.dxgiFormat;
    }

    return DXGI_FORMAT_UNKNOWN;
}

YAGE_NODISCARD static auto WICConvertFormat(const WICPixelFormatGUID &guid, WICPixelFormatGUID &target) noexcept
    -> bool {
    constexpr const struct {
        const WICPixelFormatGUID &source;
        const WICPixelFormatGUID &target;
    } convertMap[]{
        {GUID_WICPixelFormat1bppIndexed, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat2bppIndexed, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat4bppIndexed, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat8bppIndexed, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat2bppGray, GUID_WICPixelFormat8bppGray},
        {GUID_WICPixelFormat4bppGray, GUID_WICPixelFormat8bppGray},
        {GUID_WICPixelFormat16bppGrayFixedPoint, GUID_WICPixelFormat16bppGrayHalf},
        {GUID_WICPixelFormat32bppGrayFixedPoint, GUID_WICPixelFormat32bppGrayFloat},
        {GUID_WICPixelFormat16bppBGR555, GUID_WICPixelFormat16bppBGRA5551},
        {GUID_WICPixelFormat32bppBGR101010, GUID_WICPixelFormat32bppRGBA1010102},
        {GUID_WICPixelFormat24bppBGR, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat24bppRGB, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat32bppPBGRA, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat32bppPRGBA, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat48bppRGB, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat48bppBGR, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat64bppBGRA, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat64bppPRGBA, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat64bppPBGRA, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat48bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat48bppBGRFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppRGBAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppBGRAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat64bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat48bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf},
        {GUID_WICPixelFormat128bppPRGBAFloat, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat128bppRGBFloat, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat128bppRGBAFixedPoint, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat128bppRGBFixedPoint, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat32bppRGBE, GUID_WICPixelFormat128bppRGBAFloat},
        {GUID_WICPixelFormat32bppCMYK, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat64bppCMYK, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat40bppCMYKAlpha, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat80bppCMYKAlpha, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat32bppRGB, GUID_WICPixelFormat32bppRGBA},
        {GUID_WICPixelFormat64bppRGB, GUID_WICPixelFormat64bppRGBA},
        {GUID_WICPixelFormat64bppPRGBAHalf, GUID_WICPixelFormat64bppRGBAHalf},
    };

    for (const auto &c : convertMap) {
        if (guid == c.source) {
            target = c.target;
            return true;
        }
    }

    return false;
}

YAGE_NODISCARD static auto GetPixelBitSize(const WICPixelFormatGUID &format, uint32_t &bitSize) noexcept -> HRESULT {
    ComPtr<IWICComponentInfo> componentInfo;
    HRESULT hr = WICImageFactory::Singleton()->CreateComponentInfo(format, componentInfo.GetAddressOf());
    if (FAILED(hr))
        return hr;

    WICComponentType type;
    hr = componentInfo->GetComponentType(&type);
    if (FAILED(hr))
        return hr;

    if (type != WICPixelFormat)
        return WINCODEC_ERR_COMPONENTNOTFOUND;

    ComPtr<IWICPixelFormatInfo> pixelFormatInfo;
    hr = componentInfo.As(&pixelFormatInfo);
    if (FAILED(hr))
        return hr;

    hr = pixelFormatInfo->GetBitsPerPixel(&bitSize);
    if (FAILED(hr))
        return hr;

    return S_OK;
}

} // namespace

YaGE::Image::Image() noexcept : width(), height(), pixelFormat(), pixelBitSize(), rowPitch(), slicePitch(), data() {}

YaGE::Image::Image(StringView path)
    : width(), height(), pixelFormat(), pixelBitSize(), rowPitch(), slicePitch(), data() {
    HRESULT hr = S_OK;

    // Get WIC imaging factory.
    IWICImagingFactory *const wic = WICImageFactory::Singleton();

    // Load image from file.
    ComPtr<IWICBitmapDecoder> decoder;
    if (path.IsNullTerminated()) {
        hr = wic->CreateDecoderFromFilename(reinterpret_cast<LPCWSTR>(path.Data()), nullptr, GENERIC_READ,
                                            WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
    } else {
        String tempPath(path);
        hr = wic->CreateDecoderFromFilename(reinterpret_cast<LPCWSTR>(tempPath.Data()), nullptr, GENERIC_READ,
                                            WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
    }

    if (FAILED(hr))
        throw SystemErrorException(hr, Format(u"Failed to load image file: {}.", path));

    // Get frame.
    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(hr))
        throw SystemErrorException(hr, u"Failed to get image frame.");

    WICPixelFormatGUID wicFormat;
    hr = frame->GetPixelFormat(&wicFormat);
    if (FAILED(hr))
        throw SystemErrorException(hr, u"Failed to get image pixel format.");

    // Convert to DXGI format.
    ComPtr<IWICBitmapSource> bmp;

    pixelFormat = ToDXGIFormat(wicFormat);
    if (pixelFormat == DXGI_FORMAT_UNKNOWN) {
        // Get convert format.
        WICPixelFormatGUID targetFormat;
        if (!WICConvertFormat(wicFormat, targetFormat))
            throw SystemErrorException(WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT, u"Unsupported image format.");

        // Convert to target format.
        ComPtr<IWICFormatConverter> converter;
        hr = wic->CreateFormatConverter(converter.GetAddressOf());
        if (FAILED(hr))
            throw SystemErrorException(hr, u"Failed to create image format converter.");

        hr = converter->Initialize(frame.Get(), targetFormat, WICBitmapDitherTypeNone, nullptr, 0.0f,
                                   WICBitmapPaletteTypeCustom);
        if (FAILED(hr))
            throw SystemErrorException(hr, u"Failed to convert image format.");

        // Get converted image.
        hr = converter.As(&bmp);
        if (FAILED(hr))
            throw SystemErrorException(hr, u"Failed to get converted image.");

        // Update format.
        pixelFormat = ToDXGIFormat(targetFormat);
        wicFormat   = targetFormat;
        assert(pixelFormat != DXGI_FORMAT_UNKNOWN);
    } else {
        bmp = frame;
    }

    // Get metadata.
    hr = bmp->GetSize(&width, &height);
    if (FAILED(hr))
        throw SystemErrorException(hr, u"Failed to get image size.");

    // Get pixel info.
    hr = GetPixelBitSize(wicFormat, pixelBitSize);
    if (FAILED(hr))
        throw SystemErrorException(hr, u"Failed to get pixel bit size.");

    // Row pitch size. Rounded up with 1 byte.
    rowPitch   = (size_t(width) * pixelBitSize + 7) / 8;
    slicePitch = rowPitch * size_t(height);

    data.resize(slicePitch);
    bmp->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(slicePitch), data.data());
}

YaGE::Image::Image(const Image &other) noexcept
    : width(other.width),
      height(other.height),
      pixelFormat(other.pixelFormat),
      pixelBitSize(other.pixelBitSize),
      rowPitch(other.rowPitch),
      slicePitch(other.slicePitch),
      data(other.data) {}

auto YaGE::Image::operator=(const Image &other) noexcept -> Image & {
    if (this == &other)
        return *this;

    width        = other.width;
    height       = other.height;
    pixelFormat  = other.pixelFormat;
    pixelBitSize = other.pixelBitSize;
    rowPitch     = other.rowPitch;
    slicePitch   = other.slicePitch;
    data         = other.data;

    return *this;
}

YaGE::Image::Image(Image &&other) noexcept
    : width(other.width),
      height(other.height),
      pixelFormat(other.pixelFormat),
      pixelBitSize(other.pixelBitSize),
      rowPitch(other.rowPitch),
      slicePitch(other.slicePitch),
      data(std::move(other.data)) {
    other.width        = 0;
    other.height       = 0;
    other.pixelFormat  = DXGI_FORMAT_UNKNOWN;
    other.pixelBitSize = 0;
    other.rowPitch     = 0;
    other.slicePitch   = 0;
}

auto YaGE::Image::operator=(Image &&other) noexcept -> Image & {
    width        = other.width;
    height       = other.height;
    pixelFormat  = other.pixelFormat;
    pixelBitSize = other.pixelBitSize;
    rowPitch     = other.rowPitch;
    slicePitch   = other.slicePitch;
    data         = std::move(other.data);

    other.width        = 0;
    other.height       = 0;
    other.pixelFormat  = DXGI_FORMAT_UNKNOWN;
    other.pixelBitSize = 0;
    other.rowPitch     = 0;
    other.slicePitch   = 0;

    return *this;
}

YaGE::Image::~Image() noexcept {}
