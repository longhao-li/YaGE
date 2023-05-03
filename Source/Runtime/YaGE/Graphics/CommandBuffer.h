#pragma once

#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "DynamicDescriptorHeap.h"
#include "GpuBuffer.h"
#include "PipelineState.h"
#include "RenderDevice.h"
#include "Texture.h"

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
    ///   Copy data from system memory to texture.
    ///
    /// @param      width       Width of the source image.
    /// @param      height      Height of the source image.
    /// @param      srcFormat   Format of the source image.
    /// @param[in]  src         Source data to be copied from.
    /// @param      srcRowPitch Row pitch size in byte of @p src.
    /// @param[out] dest        Destination texture to be copied to.
    /// @param      mipLevel    Mip level of the destination texture to be copied to.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto CopyTexture(uint32_t     width,
                              uint32_t     height,
                              DXGI_FORMAT  srcFormat,
                              const void  *src,
                              uint32_t     srcRowPitch,
                              PixelBuffer &dest,
                              uint32_t     mipLevel) -> void;

    /// @brief
    ///   Set a single render target for current pipeline.
    /// @note
    ///   State of the specified color buffer will be automatically transitioned if necessary.
    ///
    /// @param[in] renderTarget The color buffer to be used as render target.
    YAGE_API auto SetRenderTarget(ColorBuffer &renderTarget) noexcept -> void;

    /// @brief
    ///   Set a single render target and depth buffer for current pipeline.
    /// @note
    ///   State of the specified color buffer and depth buffer will be automatically transitioned if necessary.
    ///
    /// @param[in] renderTarget The color buffer to be used as render target.
    /// @param[in] depthTarget  The depth buffer to be used as depth target.
    YAGE_API auto SetRenderTarget(ColorBuffer &renderTarget, DepthBuffer &depthTarget) noexcept -> void;

    /// @brief
    ///   Update depth buffer without rendering to any render target.
    /// @note
    ///   State of the specified depth buffer will be automatically transitioned if necessary.
    ///
    /// @param[in] depthTarget  The depth buffer to be updated.
    YAGE_API auto SetRenderTarget(DepthBuffer &depthTarget) noexcept -> void;

    /// @brief
    ///   Set multiple render targets for current pipeline without depth target.
    /// @note
    ///   State of the specified color buffers will be automatically transitioned if necessary.
    ///
    /// @param[in] count         Number of render targets. This value should be less than or equal to 8.
    /// @param[in] renderTargets Array of color buffers to be used as render targets.
    YAGE_API auto SetRenderTargets(uint32_t count, ColorBuffer **renderTargets) noexcept -> void;

    /// @brief
    ///   Set multiple render targets and depth buffer for current pipeline.
    /// @note
    ///   State of the specified color buffers and depth buffer will be automatically transitioned if necessary.
    ///
    /// @param[in] count         Number of render targets.
    /// @param[in] renderTargets Array of color buffers to be used as render targets.
    /// @param[in] depthTarget   The depth buffer to be used as depth target.
    YAGE_API auto SetRenderTargets(uint32_t count, ColorBuffer **renderTargets, DepthBuffer &depthTarget) noexcept
        -> void;

    /// @brief
    ///   Clear a color buffer to its clear color.
    ///
    /// @param[in, out] colorBuffer The color buffer to be cleared.
    auto ClearColor(ColorBuffer &colorBuffer) noexcept -> void {
        const Color &color = colorBuffer.ClearColor();
        commandList->ClearRenderTargetView(colorBuffer.RenderTargetView(), reinterpret_cast<const float *>(&color), 0,
                                           nullptr);
    }

    /// @brief
    ///   Clear a color buffer to the specified color.
    ///
    /// @param[in, out] colorBuffer The color buffer to be cleared.
    /// @param[in]      color       The color to be used to clear the color buffer.
    auto ClearColor(ColorBuffer &colorBuffer, const Color &color) noexcept -> void {
        commandList->ClearRenderTargetView(colorBuffer.RenderTargetView(), reinterpret_cast<const float *>(&color), 0,
                                           nullptr);
    }

    /// @brief
    ///   Clear depth value for the specified depth buffer. Stencil value will not be affected.
    ///
    /// @param[in] depthBuffer  The depth buffer to be cleared.
    auto ClearDepth(DepthBuffer &depthBuffer) noexcept -> void {
        commandList->ClearDepthStencilView(depthBuffer.DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH,
                                           depthBuffer.ClearDepth(), depthBuffer.ClearStencil(), 0, nullptr);
    }

    /// @brief
    ///   Clear depth value for the specified depth buffer. Stencil value will not be affected.
    ///
    /// @param[in, out] depthBuffer The depth buffer to be cleared.
    /// @param          clearDepth  The depth value to be used to clear the depth buffer.
    auto ClearDepth(DepthBuffer &depthBuffer, float clearDepth) noexcept -> void {
        commandList->ClearDepthStencilView(depthBuffer.DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH, clearDepth,
                                           depthBuffer.ClearStencil(), 0, nullptr);
    }

    /// @brief
    ///   Clear stencil value for the specified depth buffer. Depth value will not be affected.
    ///
    /// @param[in, out] depthBuffer  The depth buffer to be cleared.
    auto ClearStencil(DepthBuffer &depthBuffer) noexcept -> void {
        commandList->ClearDepthStencilView(depthBuffer.DepthStencilView(), D3D12_CLEAR_FLAG_STENCIL,
                                           depthBuffer.ClearDepth(), depthBuffer.ClearStencil(), 0, nullptr);
    }

    /// @brief
    ///   Clear stencil value for the specified depth buffer. Depth value will not be affected.
    ///
    /// @param[in, out] depthBuffer  The depth buffer to be cleared.
    /// @param          stencil      The stencil value to be used to clear the depth buffer.
    auto ClearStencil(DepthBuffer &depthBuffer, uint8_t stencil) noexcept -> void {
        commandList->ClearDepthStencilView(depthBuffer.DepthStencilView(), D3D12_CLEAR_FLAG_STENCIL,
                                           depthBuffer.ClearDepth(), stencil, 0, nullptr);
    }

    /// @brief
    ///   Clear depth and stencil value for the specified depth buffer.
    ///
    /// @param[in, out] depthBuffer  The depth buffer to be cleared.
    auto ClearDepthStencil(DepthBuffer &depthBuffer) noexcept -> void {
        commandList->ClearDepthStencilView(depthBuffer.DepthStencilView(),
                                           D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depthBuffer.ClearDepth(),
                                           depthBuffer.ClearStencil(), 0, nullptr);
    }

    /// @brief
    ///   Clear depth and stencil value for the specified depth buffer.
    ///
    /// @param[in, out] depthBuffer  The depth buffer to be cleared.
    /// @param          depth        The depth value to be used to clear the depth buffer.
    /// @param          stencil      The stencil value to be used to clear the depth buffer.
    auto ClearDepthStencil(DepthBuffer &depthBuffer, float depth, uint8_t stencil) noexcept -> void {
        commandList->ClearDepthStencilView(depthBuffer.DepthStencilView(),
                                           D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0,
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

    /// @brief
    ///   Set a vertex buffer to the specified slot.
    ///
    /// @param slot         The slot to set the vertex buffer.
    /// @param gpuAddress   The GPU address of the start of the vertex buffer.
    /// @param vertexCount  Number of vertices.
    /// @param stride       Stride size in byte of each vertex.
    auto SetVertexBuffer(uint32_t slot, uint64_t gpuAddress, uint32_t vertexCount, uint32_t stride) noexcept -> void {
        const D3D12_VERTEX_BUFFER_VIEW vbv{
            /* BufferLocation = */ gpuAddress,
            /* SizeInBytes    = */ vertexCount * stride,
            /* StrideInBytes  = */ stride,
        };
        commandList->IASetVertexBuffers(slot, 1, &vbv);
    }

    /// @brief
    ///   Use a structured buffer as vertex buffer.
    ///
    /// @param slot         The slot to set the vertex buffer.
    /// @param buffer       The structured buffer to be set.
    auto SetVertexBuffer(uint32_t slot, const StructuredBuffer &buffer) noexcept -> void {
        const D3D12_VERTEX_BUFFER_VIEW vbv{
            /* BufferLocation = */ buffer.GpuAddress(),
            /* SizeInBytes    = */ buffer.ElementSize() * buffer.ElementCount(),
            /* StrideInBytes  = */ buffer.ElementSize(),
        };

        commandList->IASetVertexBuffers(slot, 1, &vbv);
    }

    /// @brief
    ///   Use a temporary upload buffer as vertex buffer.
    /// @note
    ///   It is slow to upload vertex data to GPU for each frame. Please consider using static vertex buffer if possible.
    ///
    /// @param     slot         The slot to set the vertex buffer.
    /// @param[in] data         The vertex data to be copied.
    /// @param     vertexCount  Number of vertices to be copied.
    /// @param     stride       Stride size in byte of each vertex.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto SetVertexBuffer(uint32_t slot, const void *data, uint32_t vertexCount, uint32_t stride) -> void;

    /// @brief
    ///   Set index buffer for current draw call.
    ///
    /// @param gpuAddress   The GPU address of the start of the index buffer.
    /// @param indexCount   Number of indices.
    /// @param isUInt16     Specifies whether indices are uint16 or uint32.
    auto SetIndexBuffer(uint64_t gpuAddress, uint32_t indexCount, bool isUInt16) noexcept -> void {
        const DXGI_FORMAT             format = isUInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        const D3D12_INDEX_BUFFER_VIEW ibv{
            /* BufferLocation = */ gpuAddress,
            /* SizeInBytes    = */ indexCount * (isUInt16 ? 2U : 4U),
            /* Format         = */ format,
        };

        commandList->IASetIndexBuffer(&ibv);
    }

    /// @brief
    ///   Use a structured buffer as index buffer.
    /// @remarks
    ///   Index type is determined by element size of the structured buffer.
    ///
    /// @param[in] buffer   The structured buffer to be set.
    auto SetIndexBuffer(const StructuredBuffer &buffer) noexcept -> void {
        const DXGI_FORMAT             format = buffer.ElementSize() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        const D3D12_INDEX_BUFFER_VIEW ibv{
            /* BufferLocation = */ buffer.GpuAddress(),
            /* SizeInBytes    = */ buffer.ElementSize() * buffer.ElementCount(),
            /* Format         = */ format,
        };

        commandList->IASetIndexBuffer(&ibv);
    }

    /// @brief
    ///   Use a temporary upload buffer as index buffer.
    /// @note
    ///   It is slow to upload index data to GPU for each frame. Please consider using static index buffer if possible.
    ///
    /// @param[in] data         The index data to be copied.
    /// @param     indexCount   Number of indices to be copied.
    /// @param     isUInt16     Specifies whether indices are uint16 or uint32.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate temporary upload buffer.
    YAGE_API auto SetIndexBuffer(const void *data, uint32_t indexCount, bool isUInt16) -> void;

    /// @brief
    ///   Set pipeline state for this command buffer.
    /// @note
    ///   Root signatures will not be affected by this method. You must set root signature manually.
    ///
    /// @param pipelineState   The pipeline state to be set.
    auto SetPipelineState(const PipelineState &pso) noexcept -> void {
        commandList->SetPipelineState(pso.D3D12PipelineState());
    }

    /// @brief
    ///   Set primitive topology for current draw call.
    ///
    /// @param topology   Primitive topology for current draw call.
    auto SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) noexcept -> void {
        commandList->IASetPrimitiveTopology(topology);
    }

    /// @brief
    ///   Set viewport for current draw call.
    /// @remarks
    ///   D3D12 screen coordinate starts from top-left corner of screen.
    ///
    /// @param x        X coordinate from top-left corner of this viewport.
    /// @param y        Y coordinate from top-left corner of this viewport.
    /// @param width    Width of this viewport.
    /// @param height   Height of this viewport.
    /// @param zNear    Minimum depth of this viewport.
    /// @param zFar     Maximum depth of this viewport.
    template <typename X, typename Y, typename Width, typename Height>
    auto SetViewport(X x, Y y, Width width, Height height, float zNear = 0.0f, float zFar = 1.0f) noexcept -> void {
        static_assert(std::is_arithmetic<X>::value, "X must be arithmetic type.");
        static_assert(std::is_arithmetic<Y>::value, "Y must be arithmetic type.");
        static_assert(std::is_arithmetic<Width>::value, "Width must be arithmetic type.");
        static_assert(std::is_arithmetic<Height>::value, "Height must be arithmetic type.");

        const D3D12_VIEWPORT viewport{
            /* TopLeftX = */ static_cast<float>(x),
            /* TopLeftY = */ static_cast<float>(y),
            /* Width    = */ static_cast<float>(width),
            /* Height   = */ static_cast<float>(height),
            /* MinDepth = */ zNear,
            /* MaxDepth = */ zFar,
        };

        commandList->RSSetViewports(1, &viewport);
    }

    /// @brief
    ///   Set viewports for current draw call.
    /// @remarks
    ///   D3D12 screen coordinate starts from top-left corner of screen.
    ///
    /// @param count      Number of viewports to be set.
    /// @param viewports  Array of viewports to be set.
    auto SetViewports(uint32_t count, const D3D12_VIEWPORT *viewports) noexcept -> void {
        commandList->RSSetViewports(count, viewports);
    }

    /// @brief
    ///   Set scissor rectangle for current draw call.
    /// @remarks
    ///   D3D12 screen coordinate starts from top-left corner of screen.
    ///
    /// @param x        X coordinate from top-left corner of this scissor rectangle.
    /// @param y        Y coordinate from top-left corner of this scissor rectangle.
    /// @param width    Width of this scissor rectangle.
    /// @param height   Height of this scissor rectangle.
    template <typename X, typename Y, typename Width, typename Height>
    auto SetScissorRect(X x, Y y, Width width, Height height) noexcept -> void {
        static_assert(std::is_arithmetic<X>::value, "X must be arithmetic type.");
        static_assert(std::is_arithmetic<Y>::value, "Y must be arithmetic type.");
        static_assert(std::is_arithmetic<Width>::value, "Width must be arithmetic type.");
        static_assert(std::is_arithmetic<Height>::value, "Height must be arithmetic type.");

        const D3D12_RECT rect{
            /* left   = */ static_cast<LONG>(x),
            /* top    = */ static_cast<LONG>(y),
            /* right  = */ static_cast<LONG>(x + width),
            /* bottom = */ static_cast<LONG>(y + height),
        };

        commandList->RSSetScissorRects(1, &rect);
    }

    /// @brief
    ///   Set scissor rectangles for current draw call.
    /// @remarks
    ///   D3D12 screen coordinate starts from top-left corner of screen.
    ///
    /// @param numRects   Number of scissor rectangles.
    /// @param rects      Array of scissor rectangles.
    auto SetScissorRects(uint32_t numRects, const D3D12_RECT *rects) noexcept -> void {
        commandList->RSSetScissorRects(numRects, rects);
    }

    /// @brief
    ///   Draw primitives.
    ///
    /// @param vertexCount   Number of vertices to be drawn.
    /// @param firstVertex   Index of the first vertex to be drawn.
    auto Draw(uint32_t vertexCount, uint32_t firstVertex = 0) noexcept -> void {
        dynamicDescriptorHeap.Commit(commandList.Get());
        dynamicSamplerHeap.Commit(commandList.Get());
        commandList->DrawInstanced(vertexCount, 1, firstVertex, 0);
    }

    /// @brief
    ///   Draw primitives according to index buffer.
    ///
    /// @param indexCount    Number of indices to be drawn.
    /// @param firstIndex    Index of the first index in index buffer.
    /// @param firstVertex   Index of the first vertex to be drawn.
    auto DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex = 0) noexcept -> void {
        dynamicDescriptorHeap.Commit(commandList.Get());
        dynamicSamplerHeap.Commit(commandList.Get());
        commandList->DrawIndexedInstanced(indexCount, 1, firstIndex, firstVertex, 0);
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
