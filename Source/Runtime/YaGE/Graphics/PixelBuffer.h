#pragma once

#include "GpuResource.h"

namespace YaGE {

class PixelBuffer : public GpuResource {
protected:
    /// @brief
    ///   Create an empty pixel buffer.
    YAGE_API PixelBuffer() noexcept;

    /// @brief
    ///   Move constructor of PixelBuffer. The moved PixelBuffer will be invalidated.
    ///
    /// @param other    The PixelBuffer to be moved.
    YAGE_API PixelBuffer(PixelBuffer &&other) noexcept;

    /// @brief
    ///   Move assignment of PixelBuffer. The moved PixelBuffer will be invalidated.
    ///
    /// @param other    The PixelBuffer to be moved.
    ///
    /// @return PixelBuffer &
    ///   Return a reference to this PixelBuffer.
    YAGE_API auto operator=(PixelBuffer &&other) noexcept -> PixelBuffer &;

public:
    /// @brief
    ///   Destroy this pixel buffer.
    YAGE_API ~PixelBuffer() noexcept override;

    /// @brief
    ///   Get width in pixel of this pixel buffer.
    ///
    /// @return uint32_t
    ///   Return width in pixel of this pixel buffer.
    YAGE_NODISCARD auto Width() const noexcept -> uint32_t { return width; }

    /// @brief
    ///   Get height in pixel of this pixel buffer.
    ///
    /// @return uint32_t
    ///   Return height in pixel of this pixel buffer.
    YAGE_NODISCARD auto Height() const noexcept -> uint32_t { return height; }

    /// @brief
    ///   Get number of 2D textures in this pixel buffer.
    ///
    /// @return uint32_t
    ///   Return number of 2D textures in this pixel buffer.
    YAGE_NODISCARD auto ArraySize() const noexcept -> uint32_t { return arraySize; }

    /// @brief
    ///   Get number of samples of this pixel buffer.
    ///
    /// @return uint32_t
    ///   Return number of samples of this pixel buffer.
    YAGE_NODISCARD auto SampleCount() const noexcept -> uint32_t { return sampleCount; }

    /// @brief
    ///   Get mipmap level of this pixel buffer.
    ///
    /// @return uint32_t
    ///   Return mipmap level of this pixel buffer.
    YAGE_NODISCARD auto MipLevels() const noexcept -> uint32_t { return mipLevels; }

    /// @brief
    ///   Get pixel format of this pixel buffer.
    ///
    /// @return DXGI_FORMAT
    ///   Return pixel format of this pixel buffer.
    YAGE_NODISCARD auto PixelFormat() const noexcept -> DXGI_FORMAT { return pixelFormat; }

protected:
    /// @brief  Width in pixel of this pixel buffer.
    uint32_t width;

    /// @brief  Height in pixel of this pixel buffer.
    uint32_t height;

    /// @brief  Number of 2D textures in pixel of this pixel buffer.
    uint32_t arraySize;

    /// @brief  Number of samples of this pixel buffer.
    uint32_t sampleCount;

    /// @brief  Number of mip levels of this pixel buffer.
    uint32_t mipLevels;

    /// @brief  Pixel format of this pixel buffer.
    DXGI_FORMAT pixelFormat;
};

} // namespace YaGE
