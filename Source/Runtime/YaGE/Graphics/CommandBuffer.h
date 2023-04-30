#ifndef YAGE_GRAPHICS_COMMAND_BUFFER_H
#define YAGE_GRAPHICS_COMMAND_BUFFER_H

#include "ColorBuffer.h"
#include "DynamicDescriptorHeap.h"
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

    /// @brief
    ///   Set graphics root signature of this command buffer.
    ///
    /// @param[in] rootSig  The graphics root signature to be used by this command buffer.
    YAGE_API auto SetGraphicsRootSignature(RootSignature &rootSig) noexcept -> void;

    /// @brief
    ///   Set compute root signature of this command buffer.
    ///
    /// @param[in] rootSig  The compute root signature to be used by this command buffer.
    YAGE_API auto SetComputeRootSignature(RootSignature &rootSig) noexcept -> void;

    /// @brief
    ///   Set a non-sampler graphics descriptor table descriptor.
    ///
    /// @param rootParam    The root parameter index of the descriptor table.
    /// @param offset       Offset of the descriptor in the descriptor table.
    /// @param descriptor   The descriptor to be set.
    auto SetGraphicsDescriptor(uint32_t rootParam, uint32_t offset, CpuDescriptorHandle descriptor) noexcept -> void {
        dynamicDescriptorHeap.BindGraphicsDescriptor(rootParam, offset, descriptor);
    }

    /// @brief
    ///   Set a sampler graphics descriptor table descriptor.
    ///
    /// @param rootParam    The root parameter index of the descriptor table.
    /// @param offset       Offset of the descriptor in the descriptor table.
    /// @param descriptor   The descriptor to be set.
    auto SetGraphicsSampler(uint32_t rootParam, uint32_t offset, CpuDescriptorHandle descriptor) noexcept -> void {
        dynamicSamplerHeap.BindGraphicsDescriptor(rootParam, offset, descriptor);
    }

    /// @brief
    ///   Set graphics root constant.
    ///
    /// @param rootParam    The root parameter index of the constant value.
    /// @param offset       Offset of the constant.
    /// @param value        The value to be set.
    template <typename T>
    auto SetGraphicsConstant(uint32_t rootParam, uint32_t offset, const T &value) noexcept -> void {
        static_assert(sizeof(T) == sizeof(uint32_t), "Element must be actually 4 bytes in size.");
        static_assert(std::is_trivially_copyable<T>::value, "Element must be trivially copyable.");

        commandList->SetGraphicsRoot32BitConstant(rootParam, *reinterpret_cast<const UINT *>(std::addressof(value)),
                                                  offset);
    }

    /// @brief
    ///   Set graphics root constant based on the specified offset.
    ///
    /// @param rootParam    The root parameter index of the constant value.
    /// @param baseOffset   Base offset of the first constant.
    /// @param value        The first value to be set.
    /// @param others       The rest values to be set.
    template <typename T, typename... Others>
    auto SetGraphicsConstant(uint32_t rootParam, uint32_t baseOffset, const T &value, const Others &...others) noexcept
        -> void {
        static_assert(sizeof(T) == sizeof(uint32_t), "Elements must be actually 4 bytes in size.");
        static_assert(std::is_trivially_copyable<T>::value, "Elements must be trivially copyable.");

        this->SetGraphicsConstant(rootParam, baseOffset, value);
        this->SetGraphicsConstant(rootParam, baseOffset + 1, others...);
    }

    /// @brief
    ///   Copy data and set a constant buffer view of the specified root descriptor.
    ///
    /// @param rootParam    The root parameter index of the descriptor.
    /// @param data         The data to be copied.
    /// @param size         Size in byte of the data to be copied.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto SetGraphicsConstantBuffer(uint32_t rootParam, const void *data, size_t size) -> void;

    /// @brief
    ///   Copy data and set a constant buffer view of the specified root descriptor table.
    ///
    /// @param rootParam    The root parameter index of the descriptor table.
    /// @param offset       Offset of the descriptor in the descriptor table.
    /// @param data         The data to be copied.
    /// @param size         Size in byte of the data to be copied.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto SetGraphicsConstantBuffer(uint32_t rootParam, uint32_t offset, const void *data, size_t size) -> void;

    /// @brief
    ///   Set a non-sampler compute descriptor table descriptor.
    ///
    /// @param rootParam    The root parameter index of the descriptor table.
    /// @param offset       Offset of the descriptor in the descriptor table.
    /// @param descriptor   The descriptor to be set.
    auto SetComputeDescriptor(uint32_t rootParam, uint32_t offset, CpuDescriptorHandle descriptor) noexcept -> void {
        dynamicDescriptorHeap.BindComputeDescriptor(rootParam, offset, descriptor);
    }

    /// @brief
    ///   Set a sampler compute descriptor table descriptor.
    ///
    /// @param rootParam    The root parameter index of the descriptor table.
    /// @param offset       Offset of the descriptor in the descriptor table.
    /// @param descriptor   The descriptor to be set.
    auto SetComputeSampler(uint32_t rootParam, uint32_t offset, CpuDescriptorHandle descriptor) noexcept -> void {
        dynamicSamplerHeap.BindComputeDescriptor(rootParam, offset, descriptor);
    }

    /// @brief
    ///   Set compute root constant.
    ///
    /// @param rootParam    The root parameter index of the constant value.
    /// @param offset       Offset of the constant.
    /// @param value        The value to be set.
    template <typename T>
    auto SetComputeConstant(uint32_t rootParam, uint32_t offset, const T &value) noexcept -> void {
        static_assert(sizeof(T) == sizeof(uint32_t), "Element must be actually 4 bytes in size.");
        static_assert(std::is_trivially_copyable<T>::value, "Element must be trivially copyable.");

        commandList->SetComputeRoot32BitConstant(rootParam, *reinterpret_cast<const UINT *>(std::addressof(value)),
                                                 offset);
    }

    /// @brief
    ///   Set compute root constant based on the specified offset.
    ///
    /// @param rootParam    The root parameter index of the constant value.
    /// @param baseOffset   Base offset of the first constant.
    /// @param value        The first value to be set.
    /// @param others       The rest values to be set.
    template <typename T, typename... Others>
    auto SetComputeConstant(uint32_t rootParam, uint32_t baseOffset, const T &value, const Others &...others) noexcept
        -> void {
        static_assert(sizeof(T) == sizeof(uint32_t), "Elements must be actually 4 bytes in size.");
        static_assert(std::is_trivially_copyable<T>::value, "Elements must be trivially copyable.");

        this->SetComputeConstant(rootParam, baseOffset, value);
        this->SetComputeConstant(rootParam, baseOffset + 1, others...);
    }

    /// @brief
    ///   Copy data and set a constant buffer view of the specified root descriptor.
    ///
    /// @param rootParam    The root parameter index of the descriptor.
    /// @param data         The data to be copied.
    /// @param size         Size in byte of the data to be copied.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto SetComputeConstantBuffer(uint32_t rootParam, const void *data, size_t size) -> void;

    /// @brief
    ///   Copy data and set a constant buffer view of the specified root descriptor table.
    ///
    /// @param rootParam    The root parameter index of the descriptor table.
    /// @param offset       Offset of the descriptor in the descriptor table.
    /// @param data         The data to be copied.
    /// @param size         Size in byte of the data to be copied.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto SetComputeConstantBuffer(uint32_t rootParam, uint32_t offset, const void *data, size_t size) -> void;

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

    /// @brief  Current graphics root signature.
    RootSignature *graphicsRootSignature;

    /// @brief  Current compute root signature.
    RootSignature *computeRootSignature;

    /// @brief  Dynamic non-sampler descriptor heap.
    DynamicDescriptorHeap dynamicDescriptorHeap;

    /// @brief  Dynamic sampler descriptor heap.
    DynamicDescriptorHeap dynamicSamplerHeap;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_COMMAND_BUFFER_H
