#include "PixelBuffer.h"

using namespace YaGE;

YaGE::PixelBuffer::PixelBuffer() noexcept
    : GpuResource(), width(), height(), arraySize(), sampleCount(), mipLevels(), pixelFormat(DXGI_FORMAT_UNKNOWN) {}

YaGE::PixelBuffer::PixelBuffer(PixelBuffer &&other) noexcept
    : GpuResource(std::move(other)),
      width(other.width),
      height(other.height),
      arraySize(other.arraySize),
      sampleCount(other.sampleCount),
      mipLevels(other.mipLevels),
      pixelFormat(other.pixelFormat) {
    other.width       = 0;
    other.height      = 0;
    other.arraySize   = 0;
    other.sampleCount = 0;
    other.mipLevels   = 0;
    other.pixelFormat = DXGI_FORMAT_UNKNOWN;
}

auto YaGE::PixelBuffer::operator=(PixelBuffer &&other) noexcept -> PixelBuffer & {
    GpuResource::operator=(std::move(other));

    width       = other.width;
    height      = other.height;
    arraySize   = other.arraySize;
    sampleCount = other.sampleCount;
    mipLevels   = other.mipLevels;
    pixelFormat = other.pixelFormat;

    other.width       = 0;
    other.height      = 0;
    other.arraySize   = 0;
    other.sampleCount = 0;
    other.mipLevels   = 0;
    other.pixelFormat = DXGI_FORMAT_UNKNOWN;

    return *this;
}

YaGE::PixelBuffer::~PixelBuffer() noexcept {}
