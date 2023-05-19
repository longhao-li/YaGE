#pragma once

#include "Descriptor.h"
#include "PixelBuffer.h"

namespace YaGE {

class DepthBuffer : public PixelBuffer {
public:
    /// @brief
    ///   Create an empty depth buffer.
    YAGE_API DepthBuffer() noexcept;

    /// @brief
    ///   Create a new depth buffer.
    ///
    /// @param width        Width in pixel of the depth buffer.
    /// @param height       Height in pixel of the depth buffer.
    /// @param format       Pixel format of the depth buffer.
    /// @param sampleCount  Number of samples of the depth buffer.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource for this depth buffer.
    YAGE_API DepthBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t sampleCount = 1);

    /// @brief
    ///   Move constructor of DepthBuffer. The moved DepthBuffer will be invalidated.
    ///
    /// @param other    The DepthBuffer to be moved.
    YAGE_API DepthBuffer(DepthBuffer &&other) noexcept;

    /// @brief
    ///   Move assignment of DepthBuffer. The moved DepthBuffer will be invalidated.
    ///
    /// @param other    The DepthBuffer to be moved.
    ///
    /// @return DepthBuffer &
    ///   Return reference to this DepthBuffer.
    YAGE_API auto operator=(DepthBuffer &&other) noexcept -> DepthBuffer &;

    /// @brief
    ///   Destroy this depth buffer.
    YAGE_API ~DepthBuffer() noexcept override;

    /// @brief
    ///   Get clear depth value of this depth buffer. Default clear depth is 1.0f.
    ///
    /// @return float
    ///   Return clear depth value of this depth buffer.
    YAGE_NODISCARD auto ClearDepth() const noexcept -> float { return clearDepth; }

    /// @brief
    ///   Set a new clear depth value of this depth buffer.
    ///
    /// @param depth    New clear depth value of this depth buffer.
    auto SetClearDepth(float depth) noexcept -> void { clearDepth = depth; }

    /// @brief
    ///   Get clear stencil value of this depth buffer. Default clear stencil is 0.
    ///
    /// @return uint8_t
    ///   Return clear stencil value of this depth buffer.
    YAGE_NODISCARD auto ClearStencil() const noexcept -> uint8_t { return clearStencil; }

    /// @brief
    ///   Set a new clear stencil value of this depth buffer.
    ///
    /// @param stencil    New clear stencil value of this depth buffer.
    auto SetClearStencil(uint8_t stencil) noexcept -> void { clearStencil = stencil; }

    /// @brief
    ///   Get depth stencil view CPU descriptor handle of this depth buffer.
    ///
    /// @return CpuDescriptorHandle
    ///   Return depth stencil view CPU descriptor handle of this depth buffer.
    YAGE_NODISCARD auto DepthStencilView() const noexcept -> CpuDescriptorHandle { return dsv; }

    /// @brief
    ///   Get read-only depth view CPU descriptor handle of this depth buffer.
    ///
    /// @return CpuDescriptorHandle
    ///   Return read-only depth view CPU descriptor handle of this depth buffer.
    YAGE_NODISCARD auto DepthReadOnlyView() const noexcept -> CpuDescriptorHandle { return depthReadOnlyView; }

    /// @brief
    ///   Get depth shader resource view CPU descriptor handle of this depth buffer.
    ///
    /// @return CpuDescriptorHandle
    ///   Return depth shader resource view CPU descriptor handle of this depth buffer.
    YAGE_NODISCARD auto DepthShaderResourceView() const noexcept -> CpuDescriptorHandle { return srv; }

    /// @brief
    ///   Checks if this depth buffer supports unordered access.
    ///
    /// @return bool
    /// @retval true    This depth buffer supports unordered access.
    /// @retval false   This depth buffer does not support unordered access.
    YAGE_NODISCARD auto SupportUnorderedAccess() const noexcept -> bool { return !uav.IsNull(); }

    /// @brief
    ///   Get depth unordered access view CPU descriptor handle of this depth buffer. It is not guaranteed that this depth buffer supports unordered access.
    ///
    /// @return CpuDescriptorHandle
    ///   Return depth unordered access view CPU descriptor handle of this depth buffer.
    YAGE_NODISCARD auto DepthUnorderedAccessView() const noexcept -> CpuDescriptorHandle { return uav; }

private:
    /// @brief  Depth clear value of this depth buffer. Default value is 1.0f.
    float clearDepth;

    /// @brief  Stencil clear value of this depth buffer. Default value is 0.
    uint8_t clearStencil;

    /// @brief  Depth stencil view of this depth buffer.
    YaGE::DepthStencilView dsv;

    /// @brief  Read-only depth view of this depth buffer.
    YaGE::DepthStencilView depthReadOnlyView;

    /// @brief  Depth shader resource view of this depth buffer.
    YaGE::ShaderResourceView srv;

    /// @brief  Depth unordered access resource view of this depth buffer.
    YaGE::UnorderedAccessView uav;
};

} // namespace YaGE
