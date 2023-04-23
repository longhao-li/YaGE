#ifndef YAGE_GRAPHICS_SWAP_CHAIN_H
#define YAGE_GRAPHICS_SWAP_CHAIN_H

#include "ColorBuffer.h"
#include "RenderDevice.h"

namespace YaGE {

class SwapChain {
public:
    /// @brief
    ///   Create a swap chain for the specified window. This swap chain will take over rendering works of the window.
    ///
    /// @param window           Window to render to.
    /// @param numBuffers       Number of back buffers in the swap chain. This value will always be clamped between 2 and 3.
    /// @param bufferFormat     Pixel format of back buffers.
    /// @param enableTearing    Enable variable refresh rate (VRR) if supported. Please notice that no error will be reported if failed to enable variable refresh rate. You could use @p IsTearingEnabled() to check whether variable refresh rate is enabled.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create swap chain object for the window.
    YAGE_API SwapChain(HWND        window,
                       uint32_t    numBuffers    = 2,
                       DXGI_FORMAT bufferFormat  = DXGI_FORMAT_R8G8B8A8_UNORM,
                       bool        enableTearing = false);

    /// @brief
    ///   Copy constructor is disabled for SwapChain.
    SwapChain(const SwapChain &) = delete;

    /// @brief
    ///   Copy assignment is disabled for SwapChain.
    auto operator=(const SwapChain &) = delete;

    /// @brief
    ///   Destroy this swap chain and release all back buffers.
    /// @note
    ///   This destructor will block current thread until all pending GPU commands are finished so that back buffers could be released safely.
    YAGE_API ~SwapChain() noexcept;

    /// @brief
    ///   Present current back buffer. This method returns immediately.
    ///
    /// @return uint64_t
    ///   Return a sync point that indicates when the presented back buffer could be reused.
    YAGE_API auto Present() noexcept -> uint64_t;

    /// @brief
    ///   Resize back buffers in this swap chain.
    /// @note
    ///   This method will wait for all pending GPU commands to finish before resizing back buffers.
    ///
    /// @param width    New width of back buffers. Pass (0, 0) to use client size of the window.
    /// @param height   New height of back buffers. Pass (0, 0) to use client size of the window.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to resize back buffers or failed to retrieve new back buffers.
    YAGE_API auto Resize(uint32_t width, uint32_t height) -> void;

    /// @brief
    ///   Checks if variable refresh rate is enabled.
    ///
    /// @return bool
    /// @retval true    Variable refresh rate is enabled.
    /// @retval false   Variable refresh rate is disabled.
    YAGE_NODISCARD auto IsTearingEnabled() const noexcept -> bool { return tearingEnabled; }

    /// @brief
    ///   Get current back buffer.
    /// @note
    ///   This method will block current thread until current back buffer is available.
    ///
    /// @return ColorBuffer &
    ///   Return reference to current back buffer.
    YAGE_NODISCARD auto CurrentBackBuffer() const noexcept -> ColorBuffer & {
        renderDevice.Sync(presentSyncPoints[bufferIndex]);
        return backBuffers[bufferIndex];
    }

    /// @brief
    ///   Get pixel format of back buffers.
    ///
    /// @return DXGI_FORMAT
    ///   Return pixel format of back buffers.
    YAGE_NODISCARD auto PixelFormat() const noexcept -> DXGI_FORMAT { return pixelFormat; }

    /// @brief
    ///   Get number of back buffers in this swap chain.
    ///
    /// @return uint32_t
    ///   Return number of back buffers in this swap chain.
    YAGE_NODISCARD auto BufferCount() const noexcept -> uint32_t { return bufferCount; }

    /// @brief
    ///   Set clear color for back buffers.
    ///
    /// @param color    Back buffer new clear color.
    auto SetClearColor(const Color &color) noexcept -> void {
        for (auto &buffer : backBuffers)
            buffer.SetClearColor(color);
    }

private:
    /// @brief  The render device that created this swap chain.
    RenderDevice &renderDevice;

    /// @brief  The DXGI swap chain object.
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

    /// @brief  Indicates whether variable refresh rate is enabled.
    bool tearingEnabled;

    /// @brief  Number of back buffers in this swap chain.
    const uint32_t bufferCount;

    /// @brief  Current back buffer index.
    uint32_t bufferIndex;

    /// @brief  Pixel format of back buffers.
    const DXGI_FORMAT pixelFormat;

    /// @brief  Swap chain back buffers.
    mutable ColorBuffer backBuffers[3];

    /// @brief  Present sync points for each back buffer.
    uint64_t presentSyncPoints[3];
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_SWAP_CHAIN_H
