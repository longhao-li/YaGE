#pragma once

#include "Descriptor.h"
#include "GpuResource.h"

namespace YaGE {

class GpuBuffer : public GpuResource {
public:
    /// @brief
    ///   Create an empty GpuBuffer.
    YAGE_API GpuBuffer() noexcept;

    /// @brief
    ///   Create a new GpuBuffer with at least the given size.
    ///
    /// @param size     Expected buffer size in byte of this GpuBuffer. The actual buffer size might be greater than the given size.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource for this GpuBuffer.
    YAGE_API explicit GpuBuffer(size_t size);

    /// @brief
    ///   Move constructor of GpuBuffer. The moved GpuBuffer will be invalidated.
    ///
    /// @param other    The GpuBuffer to be moved.
    YAGE_API GpuBuffer(GpuBuffer &&other) noexcept;

    /// @brief
    ///   Move assignment operator of GpuBuffer. The moved GpuBuffer will be invalidated.
    ///
    /// @param other    The GpuBuffer to be moved.
    ///
    /// @return GpuBuffer &
    ///   Return reference to this GpuBuffer.
    YAGE_API auto operator=(GpuBuffer &&other) noexcept -> GpuBuffer &;

    /// @brief
    ///   Destroy this GpuBuffer.
    YAGE_API ~GpuBuffer() noexcept override;

    /// @brief
    ///   Get size in byte of this GpuBuffer.
    ///
    /// @return size_t
    ///   Return size in byte of this GpuBuffer.
    YAGE_NODISCARD auto Size() const noexcept -> size_t { return bufferSize; }

    /// @brief
    ///   Get GPU address to start of this GpuBuffer.
    ///
    /// @return uint64_t
    ///   Return GPU address to start of this GpuBuffer.
    YAGE_NODISCARD auto GpuAddress() const noexcept -> uint64_t { return address; }

    /// @brief
    ///   Get byte address unordered access view of this GpuBuffer. This UAV could be used as a RWByteAddressBuffer in HLSL.
    /// @note
    ///   Please notice that byte address buffer is 4-byte aligned.
    ///
    /// @return CpuDescriptorHandle
    ///   Return byte address unordered access view CPU handle of this GpuBuffer.
    YAGE_NODISCARD auto ByteAddressUnorderedAccessView() const noexcept -> CpuDescriptorHandle {
        return byteAddressUAV;
    }

private:
    /// @brief  Size in byte of this GpuBuffer.
    size_t bufferSize;

    /// @brief  GPU address to start of this GpuBuffer.
    uint64_t address;

    /// @brief  Byte address unordered access view of this GpuBuffer. This UAV could be used as a RWByteAddressBuffer in HLSL.
    YaGE::UnorderedAccessView byteAddressUAV;
};

class StructuredBuffer : public GpuBuffer {
public:
    /// @brief
    ///   Create an empty structured buffer.
    YAGE_API StructuredBuffer() noexcept;

    /// @brief
    ///   Create a structured buffer with the following settings.
    ///
    /// @param count    Number of elements in this structured buffer.
    /// @param size     Size in byte of each element.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resources for this structured buffer.
    YAGE_API StructuredBuffer(uint32_t count, uint32_t size);

    /// @brief
    ///   Move constructor of StructuredBuffer. The moved StructuredBuffer will be invalidated.
    ///
    /// @param other    The StructuredBuffer to be moved.
    YAGE_API StructuredBuffer(StructuredBuffer &&other) noexcept;

    /// @brief
    ///   Move assignment operator of StructuredBuffer. The moved StructuredBuffer will be invalidated.
    ///
    /// @param other    The StructuredBuffer to be moved.
    ///
    /// @return StructuredBuffer &
    ///   Return reference to this StructuredBuffer.
    YAGE_API auto operator=(StructuredBuffer &&other) noexcept -> StructuredBuffer &;

    /// @brief
    ///   Destroy this StructuredBuffer.
    YAGE_API ~StructuredBuffer() noexcept override;

    /// @brief
    ///   Get number of elements in this structured buffer.
    ///
    /// @return uint32_t
    ///   Return number of elements in this structured buffer.
    YAGE_NODISCARD auto ElementCount() const noexcept -> uint32_t { return elementCount; }

    /// @brief
    ///   Get size in byte of each element.
    ///
    /// @return uint32_t
    ///   Return size in byte of each element.
    YAGE_NODISCARD auto ElementSize() const noexcept -> uint32_t { return elementSize; }

    /// @brief
    ///   Get structured unordered access view of this GpuBuffer. This UAV could be used as a RWStructuredBuffer in HLSL.
    ///
    /// @return CpuDescriptorHandle
    ///   Return structured unordered access view CPU handle of this GpuBuffer.
    YAGE_NODISCARD auto StructuredUnorderedAccessView() const noexcept -> CpuDescriptorHandle {
        return structedBufferUAV;
    }

private:
    /// @brief  Size in byte of each element.
    uint32_t elementSize;

    /// @brief  Number of elements in this structured buffer.
    uint32_t elementCount;

    /// @brief  Structured buffer unordered access view of this GpuBuffer. This UAV could be used as a RWStructuredBuffer in HLSL.
    YaGE::UnorderedAccessView structedBufferUAV;
};

} // namespace YaGE
