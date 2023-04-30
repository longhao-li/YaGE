#ifndef YAGE_GRAPHICS_COLOR_BUFFER_H
#define YAGE_GRAPHICS_COLOR_BUFFER_H

#include "Color.h"
#include "Descriptor.h"
#include "PixelBuffer.h"

namespace YaGE {

class ColorBuffer : public PixelBuffer {
public:
    /// @brief
    ///   Create an empty color buffer.
    YAGE_API ColorBuffer() noexcept;

    /// @brief
    ///   Create a new color buffer.
    /// @remarks
    ///   Unordered access support is enabled if multi-sample is not enabled (sample count is 1).
    ///
    /// @param width        Width in pixel of the color buffer.
    /// @param height       Height in pixel of the color buffer.
    /// @param arraySize    Number of 2D textures in the color buffer.
    /// @param format       Pixel format of the color buffer.
    /// @param mipmapLevels Supported mipmap levels of the color buffer. This value will always be clamped between 1 and maximum supported levels. Pass 0 to use maximum supported levels.
    /// @param sampleCount  Number of samples of the color buffer.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource for this color buffer.
    YAGE_API ColorBuffer(uint32_t    width,
                         uint32_t    height,
                         uint32_t    arraySize,
                         DXGI_FORMAT format,
                         uint32_t    mipmapLevels = 1,
                         uint32_t    sampleCount  = 1);

    /// @brief
    ///   Create a new 2D color buffer.
    /// @remarks
    ///   This is the same as ColorBuffer(width, height, 1, format).
    ///
    /// @param width    Width in pixel of the color buffer.
    /// @param height   Height in pixel of the color buffer.
    /// @param format   Pixel format of the color buffer.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 resource for this color buffer.
    YAGE_API ColorBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format);

    /// @brief
    ///   Move constructor of ColorBuffer. The moved ColorBuffer will be invalidated.
    ///
    /// @param other    The ColorBuffer to be moved.
    YAGE_API ColorBuffer(ColorBuffer &&other) noexcept;

    /// @brief
    ///   Move assignment of ColorBuffer. The moved ColorBuffer will be invalidated.
    ///
    /// @param other    The ColorBuffer to be moved.
    ///
    /// @return ColorBuffer &
    ///   Return reference to this ColorBuffer.
    YAGE_API auto operator=(ColorBuffer &&other) noexcept -> ColorBuffer &;

    /// @brief
    ///   Destroy of ColorBuffer.
    YAGE_API ~ColorBuffer() noexcept override;

    /// @brief
    ///   Get clear color of this color buffer. Default clear color is transparent black.
    ///
    /// @return const Color &
    ///   Return clear color of this color buffer.
    YAGE_NODISCARD auto ClearColor() const noexcept -> const Color & { return clearColor; }

    /// @brief
    ///   Set clear color of this color buffer.
    ///
    /// @param color    New clear color of this color buffer.
    auto SetClearColor(const Color &color) noexcept -> void { clearColor = color; }

    /// @brief
    ///   Get render target view CPU descriptor handle of this color buffer.
    /// @note
    ///   It is guaranteed that all color buffers has a render target view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return render target view CPU descriptor handle of this color buffer.
    YAGE_NODISCARD auto RenderTargetView() const noexcept -> CpuDescriptorHandle { return rtv; }

    /// @brief
    ///   Get shader resource view CPU descriptor handle of this color buffer.
    /// @note
    ///   Swap chain back buffers doesn't have a shader resource view. But it is guaranteed that color buffers created from constructor have shader resource view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return shader resource view CPU descriptor handle of this color buffer.
    YAGE_NODISCARD auto ShaderResourceView() const noexcept -> CpuDescriptorHandle { return srv; }

    /// @brief
    ///   Get unordered access view CPU descriptor handle of this color buffer.
    /// @note
    ///   Multi-sample color buffers and swap chain back buffers doesn't have unordered access view. But it is guaranteed that other color buffers have unordered access view if the pixel format supports unordered access. You could only access mipmap level 0 via this unordered access view if mip-map is enabled.
    ///
    /// @return CpuDescriptorHandle
    ///   Return unordered access view CPU descriptor handle of this color buffer.
    YAGE_NODISCARD auto UnorderedAccessView() const noexcept -> CpuDescriptorHandle { return uav; }

    /// @brief
    ///   Check if this color buffer supports unordered access.
    ///
    /// @return bool
    /// @retval true    This color buffer supports unordered access.
    /// @retval false   This color buffer doesn't support unordered access.
    YAGE_NODISCARD auto SupportUnorderedAccess() const noexcept -> bool { return !uav.IsNull(); }

    friend class SwapChain;

private:
    /// @brief
    ///   For internal usage. Release swap chain resource so that swap chain could resize back buffers.
    auto ReleaseSwapChainResource() noexcept -> void;

    /// @brief
    ///   For internal usage. Reset swap chain resource for this color buffer.
    ///
    /// @param buffer   The new swap chain back buffer.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create resource views for this color buffer.
    auto ResetSwapChainResource(Microsoft::WRL::ComPtr<ID3D12Resource> buffer) -> void;

private:
    /// @brief  Clear color of this color buffer.
    Color clearColor;

    /// @brief  Render target view that refers to the whole color buffer.
    YaGE::RenderTargetView rtv;

    /// @brief  Shader resource view that refers to the whole color buffer.
    YaGE::ShaderResourceView srv;

    /// @brief  Unordered access view that refers to the whole color buffer. This is created only when multi-sample is not enabled. No counter resource is specified.
    YaGE::UnorderedAccessView uav;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_COLOR_BUFFER_H
