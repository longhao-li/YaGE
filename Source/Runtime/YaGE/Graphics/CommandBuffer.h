#ifndef YAGE_GRAPHICS_COMMAND_BUFFER_H
#define YAGE_GRAPHICS_COMMAND_BUFFER_H

#include "ColorBuffer.h"
#include "RenderDevice.h"

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>

namespace YaGE {

class CommandBuffer {
private:
    struct TempBufferAllocation {
        /// @brief  The GPU resource that this allocation belongs to.
        GpuResource *resource;

        /// @brief  Size in byte of this allocation.
        size_t size;

        /// @brief  Offset from start of the resource.
        size_t offset;

        /// @brief  CPU pointer to start of this allocation. Unordered access buffer allocation doesn't have CPU pointer.
        void *data;

        /// @brief  GPU address to start of this allocation.
        uint64_t gpuAddress;
    };

    class TempBufferAllocator {
    public:
        /// @brief
        ///   Create a temp buffer allocator.
        TempBufferAllocator() noexcept;

        /// @brief
        ///   Copy constructor of TempBufferAllocator is disabled.
        TempBufferAllocator(const TempBufferAllocator &) = delete;

        /// @brief
        ///   Copy assignment of TempBufferAllocator is disabled.
        auto operator=(const TempBufferAllocator &) = delete;

        /// @brief
        ///   Destroy this temp buffer allocator.
        ~TempBufferAllocator() noexcept;

        /// @brief
        ///   Allocate a temporary upload buffer.
        /// @remarks
        ///   It is guaranteed that the allocation is aligned up with 256 bytes.
        ///
        /// @param size     Expected size in byte of the temp buffer to be allocated.
        ///
        /// @throw RenderAPIException
        ///   Thrown if failed to create new temporary upload page.
        YAGE_NODISCARD auto AllocateUploadBuffer(size_t size) -> TempBufferAllocation;

        /// @brief
        ///   Allocate a temporary unordered access buffer.
        /// @remarks
        ///   It is guaranteed that the allocation is aligned up with 256 bytes.
        ///
        /// @param size     Expected size in byte of the temp buffer to be allocated.
        ///
        /// @throw RenderAPIException
        ///   Thrown if failed to create new temporary unordered access page.
        YAGE_NODISCARD auto AllocateUnorderedAccessBuffer(size_t size) -> TempBufferAllocation;

        /// @brief
        ///   Clean up all retired pages.
        ///
        /// @param syncPoint    The sync point that indicates when all retired pages can be reused.
        auto CleanUp(uint64_t syncPoint) noexcept -> void;

    private:
        /// @brief  Current temp upload buffer page. This is type-erased pointer.
        void *uploadPage;

        /// @brief  Offset from start of the upload page.
        size_t uploadPageOffset;

        /// @brief  Retired upload pages.
        std::vector<void *> retiredUploadPages;

        /// @brief  Current unordered access buffer page. This is type-erased pointer.
        void *unorderedAccessPage;

        /// @brief  Offset from start of the unordered access page.
        size_t unorderedAccessPageOffset;

        /// @brief  Retired unordered access pages.
        std::vector<void *> retiredUnorderedAccessPages;
    };

public:
    /// @brief
    ///   Create a new CommandBuffer.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create new command buffer or failed to acquire new command allocator.
    YAGE_API CommandBuffer();

    /// @brief
    ///   Destroy this CommandBuffer.
    YAGE_API ~CommandBuffer() noexcept;

    /// @brief
    ///   Submit this command buffer to start executing on GPU.
    /// @remarks
    ///   This method will automatically reset current command buffer once submission is done.
    ///
    /// @return uint64_t
    ///   Return a sync point that indicates when this command buffer will finish executing on GPU.
    /// @throw RenderAPIException
    ///   Thrown if failed to acquire new command allocator.
    YAGE_API auto Submit() -> uint64_t;

    /// @brief
    ///   Reset this command buffer and clean up all recorded commands.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to acquire new command allocator.
    YAGE_API auto Reset() -> void;

    /// @brief
    ///   Wait for last submission finishes executing on GPU.
    auto WaitForComplete() const noexcept -> void { renderDevice.Sync(lastSubmitSyncPoint); }

    /// @brief
    ///   Transition the specified resource to new state.
    ///
    /// @param[in] resource The GPU resource to be transitioned.
    /// @param     newState The new state of the resource.
    YAGE_API auto Transition(GpuResource &resource, D3D12_RESOURCE_STATES newState) noexcept -> void;

    /// @brief
    ///   Copy all data from @p src to @p dest.
    /// @remarks
    ///   Status of @p src and @p dest will be automatically transitioned if necessary.
    ///
    /// @param[in]  src     Source resource to be copied from.
    /// @param[out] dest    Destination resource to be copied to.
    YAGE_API auto Copy(GpuResource &src, GpuResource &dest) noexcept -> void;

    /// @brief
    ///   Copy @p size bytes of data from one buffer to another.
    ///
    /// @param[in]  src         Source buffer to be copied from.
    /// @param      srcOffset   Offset from start of @p src to start of the copy.
    /// @param[out] dest        Destination buffer to be copied to.
    /// @param      destOffset  Offset from start of @p dest to start of the copy.
    /// @param      size        Size in byte of data to be copied.
    YAGE_API auto
    CopyBuffer(GpuResource &src, size_t srcOffset, GpuResource &dest, size_t destOffset, size_t size) noexcept -> void;

    /// @brief
    ///   Copy @p size bytes of data from system memory to a buffer.
    ///
    /// @param[in]  src     Source data to be copied from.
    /// @param[out] dest    Destination buffer to be copied to.
    /// @param      offset  Offset from start of @p dest to start of the copy.
    /// @param      size    Size in byte of data to be copied.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto CopyBuffer(const void *src, GpuResource &dest, size_t destOffset, size_t size) -> void;

    /// @brief
    ///   Set a single render target for current pipeline.
    /// @note
    ///   State of the specified color buffer will be automatically transitioned if necessary.
    ///
    /// @param[in] renderTarget The color buffer to be used as render target.
    YAGE_API auto SetRenderTarget(ColorBuffer &renderTarget) noexcept -> void;

    /// @brief
    ///   Clear a color buffer to its clear color.
    ///
    /// @param[in] colorBuffer  The color buffer to be cleared.
    auto ClearColor(const ColorBuffer &colorBuffer) noexcept -> void {
        const Color &color = colorBuffer.ClearColor();
        commandList->ClearRenderTargetView(colorBuffer.RenderTargetView(), reinterpret_cast<const float *>(&color), 0,
                                           nullptr);
    }

    /// @brief
    ///   Clear a color buffer to the specified color.
    ///
    /// @param[in] colorBuffer  The color buffer to be cleared.
    /// @param[in] color        The color to be used to clear the color buffer.
    auto ClearColor(const ColorBuffer &colorBuffer, const Color &color) noexcept -> void {
        commandList->ClearRenderTargetView(colorBuffer.RenderTargetView(), reinterpret_cast<const float *>(&color), 0,
                                           nullptr);
    }

private:
    /// @brief  The render device that is used to create this command buffer.
    RenderDevice &renderDevice;

    /// @brief  Direct D3D12 command list.
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

    /// @brief  Current command allocator used by the command list.
    ID3D12CommandAllocator *allocator;

    /// @brief  The sync point that indicates when last submittion of this command buffer will be finished.
    uint64_t lastSubmitSyncPoint;

    /// @brief  Temp buffer allocator that is used to allocate temporary upload and unordered access buffers.
    TempBufferAllocator tempBufferAllocator;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_COMMAND_BUFFER_H
