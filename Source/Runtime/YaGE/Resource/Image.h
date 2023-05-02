#pragma once

#include "../Core/StringView.h"

#include <dxgiformat.h>

namespace YaGE {

class Image {
public:
    /// @brief
    ///   Create an empty 2D image.
    YAGE_API Image() noexcept;

    /// @brief
    ///   Load an image from file.
    /// @note
    ///   This constructor may take a lot of time to load image from disk to memory.
    ///
    /// @param path     Path to the file to be used as image.
    ///
    /// @throw SystemErrorException
    ///   Thrown if failed to load the image.
    YAGE_API explicit Image(StringView path);

    /// @brief
    ///   Copy constructor. Copy data from another image.
    /// @note
    ///   Images are usually huge objects and may take a long time to copy.
    ///
    /// @param other    Another image to copy from.
    YAGE_API Image(const Image &other) noexcept;

    /// @brief
    ///   Copy assignment. Copy data from another image.
    /// @note
    ///   Images are usually huge objects and may take a long time to copy.
    ///
    /// @param other    Another image to copy from.
    ///
    /// @return Image &
    ///   Return reference to this image.
    YAGE_API auto operator=(const Image &other) noexcept -> Image &;

    /// @brief
    ///   Move constructor. The moved image will be empty.
    ///
    /// @param other    The image to move from.
    YAGE_API Image(Image &&other) noexcept;

    /// @brief
    ///   Move assignment. The moved image will be empty.
    ///
    /// @param other    The image to move from.
    ///
    /// @return Image &
    ///   Return reference to this image.
    YAGE_API auto operator=(Image &&other) noexcept -> Image &;

    /// @brief
    ///   Destroy this image and release all data.
    YAGE_API ~Image() noexcept;

    /// @brief
    ///   Get width in pixel of this image.
    ///
    /// @return uint32_t
    ///   Return width in pixel of this image.
    YAGE_NODISCARD auto Width() const noexcept -> uint32_t { return width; }

    /// @brief
    ///   Get height in pixel of this image.
    ///
    /// @return uint32_t
    ///   Return height in pixel of this image.
    YAGE_NODISCARD auto Height() const noexcept -> uint32_t { return height; }

    /// @brief
    ///   Get pixel format of this image.
    ///
    /// @return DXGI_FORMAT
    ///   Return pixel format of this image.
    YAGE_NODISCARD auto PixelFormat() const noexcept -> DXGI_FORMAT { return pixelFormat; }

    /// @brief
    ///   Get number of bits per pixel of this image.
    ///
    /// @return uint32_t
    ///   Return number of bits per pixel of this image.
    YAGE_NODISCARD auto PixelBitSize() const noexcept -> uint32_t { return pixelBitSize; }

    /// @brief
    ///   Get number of bytes per row of this image.
    ///
    /// @return size_t
    ///   Return number of bytes per row of this image.
    YAGE_NODISCARD auto RowPitch() const noexcept -> size_t { return rowPitch; }

    /// @brief
    ///   Get size in byte of this image. This is equal to RowPitch() * Height().
    ///
    /// @return size_t
    ///   Return size in byte of this image.
    YAGE_NODISCARD auto Size() const noexcept -> size_t { return slicePitch; }

    /// @brief
    ///   Get image data.
    ///
    /// @return const void *
    ///   Return pointer to image data.
    YAGE_NODISCARD auto Data() const noexcept -> const void * { return data.data(); }

private:
    /// @brief  Width in pixel of this image.
    uint32_t width;

    /// @brief  Height in pixel of this image.
    uint32_t height;

    /// @brief  Pixel format of this image.
    DXGI_FORMAT pixelFormat;

    /// @brief  Number of bits per pixel of this image.
    uint32_t pixelBitSize;

    /// @brief  Number of bytes per row of this image.
    size_t rowPitch;

    /// @brief  Number of bytes of this image.
    size_t slicePitch;

    /// @brief  Image data.
    std::vector<uint8_t> data;
};

} // namespace YaGE
