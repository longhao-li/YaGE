#ifndef YAGE_GRAPHICS_DESCRIPTOR_H
#define YAGE_GRAPHICS_DESCRIPTOR_H

#include "../Core/Common.h"

#include <concurrent_queue.h>
#include <d3d12.h>
#include <wrl/client.h>

#include <mutex>
#include <stack>

namespace YaGE {

class CpuDescriptorHandle : public D3D12_CPU_DESCRIPTOR_HANDLE {
public:
    /// @brief
    ///   Create a null CPU descriptor handle.
    constexpr CpuDescriptorHandle() noexcept : D3D12_CPU_DESCRIPTOR_HANDLE{SIZE_T(-1)} {}

    /// @brief
    ///   Create a CPU descriptor handle from another descriptor handle.
    ///
    /// @param handle   A D3D12 CPU descriptor handle.
    constexpr CpuDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) noexcept : D3D12_CPU_DESCRIPTOR_HANDLE(handle) {}

    /// @brief
    ///   Checks if this descriptor handle is null.
    ///
    /// @return bool
    /// @retval true    This is a null CPU descriptor handle.
    /// @retval false   This is not a null CPU descriptor handle.
    constexpr auto IsNull() const noexcept -> bool { return ptr == SIZE_T(-1); }

    /// @brief
    ///   Apply an offset to this CPU descriptor handle.
    ///
    /// @param offset   The offset to be applied.
    ///
    /// @return CpuDescriptorHandle &
    ///   Return reference to this descriptor handle.
    constexpr auto operator+=(ptrdiff_t offset) noexcept -> CpuDescriptorHandle & {
        if (IsNull())
            return *this;

        ptr = static_cast<SIZE_T>(static_cast<ptrdiff_t>(ptr) + offset);
        return *this;
    }

    /// @brief
    ///   Apply an offset to this CPU descriptor handle.
    ///
    /// @param offset   The offset to be applied.
    ///
    /// @return CpuDescriptorHandle &
    ///   Return reference to this descriptor handle.
    constexpr auto operator-=(ptrdiff_t offset) noexcept -> CpuDescriptorHandle & {
        if (IsNull())
            return *this;

        ptr = static_cast<SIZE_T>(static_cast<ptrdiff_t>(ptr) - offset);
        return *this;
    }

    constexpr auto operator==(CpuDescriptorHandle rhs) const noexcept -> bool { return ptr == rhs.ptr; }
    constexpr auto operator!=(CpuDescriptorHandle rhs) const noexcept -> bool { return ptr != rhs.ptr; }
    constexpr auto operator<(CpuDescriptorHandle rhs) const noexcept -> bool { return ptr < rhs.ptr; }
    constexpr auto operator<=(CpuDescriptorHandle rhs) const noexcept -> bool { return ptr <= rhs.ptr; }
    constexpr auto operator>(CpuDescriptorHandle rhs) const noexcept -> bool { return ptr > rhs.ptr; }
    constexpr auto operator>=(CpuDescriptorHandle rhs) const noexcept -> bool { return ptr >= rhs.ptr; }
};

constexpr auto operator+(CpuDescriptorHandle lhs, ptrdiff_t rhs) noexcept -> CpuDescriptorHandle {
    lhs += rhs;
    return lhs;
}

constexpr auto operator+(ptrdiff_t lhs, CpuDescriptorHandle rhs) noexcept -> CpuDescriptorHandle {
    rhs += lhs;
    return rhs;
}

constexpr auto operator-(CpuDescriptorHandle lhs, ptrdiff_t rhs) noexcept -> CpuDescriptorHandle {
    lhs -= rhs;
    return lhs;
}

class DescriptorHandle {
public:
    /// @brief
    ///   Create a null shader visible descriptor handle.
    constexpr DescriptorHandle() noexcept : cpuHandle{SIZE_T(-1)}, gpuHandle{UINT64(-1)} {}

    /// @brief
    ///   Create a descriptor handle from CPU and GPU handles.
    ///
    /// @param cpuHandle    CPU descriptor handle.
    /// @param gpuHandle    GPU descriptor handle.
    constexpr DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) noexcept
        : cpuHandle(cpuHandle), gpuHandle(gpuHandle) {}

    /// @brief
    ///   Apply an offset to this descriptor handle.
    ///
    /// @param offset   The offset to be applied.
    ///
    /// @return DescriptorHandle &
    ///   Return reference to this descriptor handle.
    constexpr auto operator+=(ptrdiff_t offset) noexcept -> DescriptorHandle & {
        if (cpuHandle.ptr != SIZE_T(-1))
            cpuHandle.ptr = static_cast<SIZE_T>(static_cast<ptrdiff_t>(cpuHandle.ptr) + offset);
        if (gpuHandle.ptr != UINT64(-1))
            gpuHandle.ptr = static_cast<UINT64>(static_cast<ptrdiff_t>(gpuHandle.ptr) + offset);
        return *this;
    }

    /// @brief
    ///   Apply an offset to this descriptor handle.
    ///
    /// @param offset   The offset to be applied.
    ///
    /// @return DescriptorHandle &
    ///   Return reference to this descriptor handle.
    constexpr auto operator-=(ptrdiff_t offset) noexcept -> DescriptorHandle & {
        if (cpuHandle.ptr != SIZE_T(-1))
            cpuHandle.ptr = static_cast<SIZE_T>(static_cast<ptrdiff_t>(cpuHandle.ptr) - offset);
        if (gpuHandle.ptr != UINT64(-1))
            gpuHandle.ptr = static_cast<UINT64>(static_cast<ptrdiff_t>(gpuHandle.ptr) - offset);
        return *this;
    }

    /// @brief
    ///   Allow implicit conversion to D3D12_CPU_DESCRIPTOR_HANDLE.
    operator D3D12_CPU_DESCRIPTOR_HANDLE() const noexcept { return cpuHandle; }

    /// @brief
    ///   Allow implicit conversion to D3D12_GPU_DESCRIPTOR_HANDLE.
    operator D3D12_GPU_DESCRIPTOR_HANDLE() const noexcept { return gpuHandle; }

private:
    /// @brief  CPU descriptor handle for this shader visible descriptor.
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;

    /// @brief  GPU descriptor handle for this shader visible descriptor.
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};

constexpr auto operator+(DescriptorHandle lhs, ptrdiff_t rhs) noexcept -> DescriptorHandle {
    lhs += rhs;
    return lhs;
}

constexpr auto operator+(ptrdiff_t lhs, DescriptorHandle rhs) noexcept -> DescriptorHandle {
    rhs += lhs;
    return rhs;
}

constexpr auto operator-(DescriptorHandle lhs, ptrdiff_t rhs) noexcept -> DescriptorHandle {
    lhs -= rhs;
    return lhs;
}

class CpuDescriptorAllocator {
public:
    /// @brief
    ///   Create and initialize this CPU descriptor allocator. This object must be manually initialized before using.
    YAGE_API CpuDescriptorAllocator() noexcept;

    /// @brief
    ///   Destroy this CPU descriptor allocator and free all descriptor heaps.
    YAGE_API ~CpuDescriptorAllocator() noexcept;

    /// @brief
    ///   Initialize this CPU descriptor allocator. Duplicate initialization check is not performed.
    ///
    /// @param dev              D3D12 device that is used to create new descriptor heaps.
    /// @param descriptorType   Type of CPU descriptor.
    YAGE_API auto Initialize(ID3D12Device1 *dev, D3D12_DESCRIPTOR_HEAP_TYPE descriptorType) noexcept -> void;

    /// @brief
    ///   Allocate a new CPU descriptor.
    /// @note
    ///   A new descriptor heap will be created if there is no more free descriptors.
    ///
    /// @return CpuDescriptorHandle
    ///   Return a free CPU descriptor handle.
    /// @throw RenderAPIException
    ///   Thrown if failed to create new descriptor heap.
    YAGE_NODISCARD YAGE_API auto Allocate() -> CpuDescriptorHandle;

    /// @brief
    ///   Free the specified CPU descriptor handle.
    ///
    /// @param handle   The CPU descriptor handle to be freed.
    auto Free(CpuDescriptorHandle handle) noexcept -> void { freeHandles.push(handle); }

    /// @brief
    ///   Get the descriptor heap type of this CPU descriptor allocator.
    ///
    /// @return D3D12_DESCRIPTOR_HEAP_TYPE
    ///   Return the descriptor heap type of this CPU descriptor allocator.
    YAGE_NODISCARD auto DescriptorType() const noexcept -> D3D12_DESCRIPTOR_HEAP_TYPE { return heapType; }

private:
    /// @brief  D3D12 device that is used to create new descriptor heaps.
    ID3D12Device1 *device;

    /// @brief  Descriptor heap type.
    D3D12_DESCRIPTOR_HEAP_TYPE heapType;

    /// @brief  Increment size of this type of descriptor.
    uint32_t descriptorSize;

    /// @brief  Used to cache all descriptor heaps to avoid being destroyed.
    std::stack<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> heapPool;

    /// @brief  Used to protect heap pool.
    mutable std::mutex heapPoolMutex;

    /// @brief  Free CPU descriptor handles.
    ::Concurrency::concurrent_queue<CpuDescriptorHandle> freeHandles;

    /// @brief  Current CPU descriptor handle in the last free heap.
    CpuDescriptorHandle currentHandle;

    /// @brief  Number of free CPU descriptors in the last free heap.
    uint32_t freeDescriptorCount;
};

class ConstantBufferView {
public:
    /// @brief
    ///   Create a null constant buffer view.
    ConstantBufferView() noexcept : handle() {}

    /// @brief
    ///   Copy constructor is disabled.
    ConstantBufferView(const ConstantBufferView &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const ConstantBufferView &) = delete;

    /// @brief
    ///   Move constructor. The moved constant buffer view will be invalidated.
    ///
    /// @param other    The constant buffer view to be moved.
    ConstantBufferView(ConstantBufferView &&other) noexcept : handle(other.handle) {
        other.handle = CpuDescriptorHandle();
    }

    /// @brief
    ///   Move assignment. The moved constant buffer view will be invalidated.
    ///
    /// @param other    The constant buffer view to be moved.
    YAGE_API auto operator=(ConstantBufferView &&other) noexcept -> ConstantBufferView &;

    /// @brief
    ///   Destroy this constant buffer view and free the descriptor.
    YAGE_API ~ConstantBufferView() noexcept;

    /// @brief
    ///   Create a constant buffer view. A new CPU descriptor handle will be allocated if this is a null constant buffer view.
    ///
    /// @param gpuAddress   GPU address of the constant buffer.
    /// @param size         Size in byte of the constant buffer.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto Create(uint64_t gpuAddress, uint32_t size) -> void;

    /// @brief
    ///   Create a constant buffer view. A new CPU descriptor handle will be allocated if this is a null constant buffer view.
    ///
    /// @param desc     Describes how to create this constant buffer view.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto Create(const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) -> void;

    /// @brief
    ///   Checks if this is a null constant buffer view.
    ///
    /// @return bool
    /// @retval true   This is a null constant buffer view.
    /// @retval false  This is not a null constant buffer view.
    YAGE_NODISCARD auto IsNull() const noexcept -> bool { return handle.IsNull(); }

    /// @brief
    ///   Get CPU descriptor handle of this constant buffer view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return CPU descriptor handle of this constant buffer view.
    YAGE_NODISCARD auto DescriptorHandle() const noexcept -> CpuDescriptorHandle { return handle; }

    /// @brief
    ///   Allow implicit conversion to CPU descriptor handle.
    operator CpuDescriptorHandle() const noexcept { return handle; }

private:
    /// @brief  Descriptor handle of this constant buffer view.
    CpuDescriptorHandle handle;
};

class ShaderResourceView {
public:
    /// @brief  Create a null shader resource view.
    ShaderResourceView() noexcept : handle() {}

    /// @brief
    ///   Copy constructor is disabled.
    ShaderResourceView(const ShaderResourceView &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const ShaderResourceView &) = delete;

    /// @brief
    ///   Move constructor. The moved shader resource view will be invalidated.
    ///
    /// @param other    The shader resource view to be moved.
    ShaderResourceView(ShaderResourceView &&other) noexcept : handle(other.handle) {
        other.handle = CpuDescriptorHandle();
    }

    /// @brief
    ///   Move assignment. The moved shader resource view will be invalidated.
    ///
    /// @param other    The shader resource view to be moved.
    YAGE_API auto operator=(ShaderResourceView &&other) noexcept -> ShaderResourceView &;

    /// @brief
    ///   Destroy this shader resource view and free the descriptor.
    YAGE_API ~ShaderResourceView() noexcept;

    /// @brief
    ///   Create a shader resource view. A new CPU descriptor handle will be allocated if this is a null shader resource view.
    ///
    /// @param resource     Resource to be viewed.
    /// @param desc         Describes how to create this shader resource view.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto Create(ID3D12Resource *resource, const D3D12_SHADER_RESOURCE_VIEW_DESC &desc) -> void;

    /// @brief
    ///   Checks if this is a null shader resource view.
    ///
    /// @return bool
    /// @retval true   This is a null shader resource view.
    /// @retval false  This is not a null shader resource view.
    YAGE_NODISCARD auto IsNull() const noexcept -> bool { return handle.IsNull(); }

    /// @brief
    ///   Get CPU descriptor handle of this shader resource view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return CPU descriptor handle of this shader resource view.
    YAGE_NODISCARD auto DescriptorHandle() const noexcept -> CpuDescriptorHandle { return handle; }

    /// @brief
    ///   Allow implicit conversion to CPU descriptor handle.
    operator CpuDescriptorHandle() const noexcept { return handle; }

private:
    /// @brief  Descriptor handle of this shader resource view.
    CpuDescriptorHandle handle;
};

class UnorderedAccessView {
public:
    /// @brief  Create a null unordered access view.
    UnorderedAccessView() noexcept : handle() {}

    /// @brief
    ///   Copy constructor is disabled.
    UnorderedAccessView(const UnorderedAccessView &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const UnorderedAccessView &) = delete;

    /// @brief
    ///   Move constructor. The moved unordered access view will be invalidated.
    ///
    /// @param other    The unordered access view to be moved.
    UnorderedAccessView(UnorderedAccessView &&other) noexcept : handle(other.handle) {
        other.handle = CpuDescriptorHandle();
    }

    /// @brief
    ///   Move assignment. The moved unordered access view will be invalidated.
    ///
    /// @param other    The unordered access view to be moved.
    YAGE_API auto operator=(UnorderedAccessView &&other) noexcept -> UnorderedAccessView &;

    /// @brief
    ///   Destroy this unordered access view and free the descriptor.
    YAGE_API ~UnorderedAccessView() noexcept;

    /// @brief
    ///   Create an unordered access view. A new CPU descriptor handle will be allocated if this is a null unordered access view.
    /// @note
    ///   No counter resource is specified.
    ///
    /// @param resource     Resource to be viewed.
    /// @param desc         Describes how to create this unordered access view.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto Create(ID3D12Resource *resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc) -> void;

    /// @brief
    ///   Create an unordered access view. A new CPU descriptor handle will be allocated if this is a null unordered access view.
    ///
    /// @param resource     Resource to be viewed.
    /// @param counter      Counter resource.
    /// @param desc         Describes how to create this unordered access view.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto
    Create(ID3D12Resource *resource, ID3D12Resource *counter, const D3D12_UNORDERED_ACCESS_VIEW_DESC &desc) -> void;

    /// @brief
    ///   Checks if this is a null unordered access view.
    ///
    /// @return bool
    /// @retval true   This is a null unordered access view.
    /// @retval false  This is not a null unordered access view.
    YAGE_NODISCARD auto IsNull() const noexcept -> bool { return handle.IsNull(); }

    /// @brief
    ///   Get CPU descriptor handle of this unordered access view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return CPU descriptor handle of this unordered access view.
    YAGE_NODISCARD auto DescriptorHandle() const noexcept -> CpuDescriptorHandle { return handle; }

    /// @brief
    ///   Allow implicit conversion to CPU descriptor handle.
    operator CpuDescriptorHandle() const noexcept { return handle; }

private:
    /// @brief  Descriptor handle of this unordered access view.
    CpuDescriptorHandle handle;
};

class SamplerView {
public:
    /// @brief  Create a null sampler view.
    SamplerView() noexcept : handle() {}

    /// @brief
    ///   Copy constructor is disabled.
    SamplerView(const SamplerView &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const SamplerView &) = delete;

    /// @brief
    ///   Move constructor. The moved sampler view will be invalidated.
    ///
    /// @param other    The sampler view to be moved.
    SamplerView(SamplerView &&other) noexcept : handle(other.handle) { other.handle = CpuDescriptorHandle(); }

    /// @brief
    ///   Move assignment. The moved sampler view will be invalidated.
    ///
    /// @param other    The sampler view to be moved.
    YAGE_API auto operator=(SamplerView &&other) noexcept -> SamplerView &;

    /// @brief
    ///   Destroy this sampler view and free the descriptor.
    YAGE_API ~SamplerView() noexcept;

    /// @brief
    ///   Create a sampler view. A new CPU descriptor handle will be allocated if this is a null sampler view.
    ///
    /// @param desc        Describes how to create this sampler view.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto Create(const D3D12_SAMPLER_DESC &desc) -> void;

    /// @brief
    ///   Checks if this is a null sampler view.
    ///
    /// @return bool
    /// @retval true   This is a null sampler view.
    /// @retval false  This is not a null sampler view.
    YAGE_NODISCARD auto IsNull() const noexcept -> bool { return handle.IsNull(); }

    /// @brief
    ///   Get CPU descriptor handle of this sampler view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return CPU descriptor handle of this sampler view.
    YAGE_NODISCARD auto DescriptorHandle() const noexcept -> CpuDescriptorHandle { return handle; }

    /// @brief
    ///   Allow implicit conversion to CPU descriptor handle.
    operator CpuDescriptorHandle() const noexcept { return handle; }

private:
    /// @brief  Descriptor handle of this sampler view.
    CpuDescriptorHandle handle;
};

class RenderTargetView {
public:
    /// @brief  Create a null render target view.
    RenderTargetView() noexcept : handle() {}

    /// @brief
    ///   Copy constructor is disabled.
    RenderTargetView(const RenderTargetView &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const RenderTargetView &) = delete;

    /// @brief
    ///   Move constructor. The moved render target view will be invalidated.
    ///
    /// @param other    The render target view to be moved.
    RenderTargetView(RenderTargetView &&other) noexcept : handle(other.handle) { other.handle = CpuDescriptorHandle(); }

    /// @brief
    ///   Move assignment. The moved render target view will be invalidated.
    ///
    /// @param other    The render target view to be moved.
    YAGE_API auto operator=(RenderTargetView &&other) noexcept -> RenderTargetView &;

    /// @brief
    ///   Destroy this render target view and free the descriptor.
    YAGE_API ~RenderTargetView() noexcept;

    /// @brief
    ///   Create a render target view. A new CPU descriptor handle will be allocated if this is a null render target view.
    ///
    /// @param resource     Resource to be viewed.
    /// @param desc         Describes how to create this render target view.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto Create(ID3D12Resource *resource, const D3D12_RENDER_TARGET_VIEW_DESC &desc) -> void;

    /// @brief
    ///   Checks if this is a null render target view.
    ///
    /// @return bool
    /// @retval true   This is a null render target view.
    /// @retval false  This is not a null render target view.
    YAGE_NODISCARD auto IsNull() const noexcept -> bool { return handle.IsNull(); }

    /// @brief
    ///   Get CPU descriptor handle of this render target view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return CPU descriptor handle of this render target view.
    YAGE_NODISCARD auto DescriptorHandle() const noexcept -> CpuDescriptorHandle { return handle; }

    /// @brief
    ///   Allow implicit conversion to CPU descriptor handle.
    operator CpuDescriptorHandle() const noexcept { return handle; }

private:
    /// @brief  Descriptor handle of this render target view.
    CpuDescriptorHandle handle;
};

class DepthStencilView {
public:
    /// @brief  Create a null depth stencil view.
    DepthStencilView() noexcept : handle() {}

    /// @brief
    ///   Copy constructor is disabled.
    DepthStencilView(const DepthStencilView &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const DepthStencilView &) = delete;

    /// @brief
    ///   Move constructor. The moved depth stencil view will be invalidated.
    ///
    /// @param other    The depth stencil view to be moved.
    DepthStencilView(DepthStencilView &&other) noexcept : handle(other.handle) { other.handle = CpuDescriptorHandle(); }

    /// @brief
    ///   Move assignment. The moved depth stencil view will be invalidated.
    ///
    /// @param other    The depth stencil view to be moved.
    YAGE_API auto operator=(DepthStencilView &&other) noexcept -> DepthStencilView &;

    /// @brief
    ///   Destroy this depth stencil view and free the descriptor.
    YAGE_API ~DepthStencilView() noexcept;

    /// @brief
    ///   Create a depth stencil view. A new CPU descriptor handle will be allocated if this is a null depth stencil view.
    ///
    /// @param resource     Resource to be viewed.
    /// @param desc         Describes how to create this depth stencil view.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to allocate new descriptor.
    YAGE_API auto Create(ID3D12Resource *resource, const D3D12_DEPTH_STENCIL_VIEW_DESC &desc) -> void;

    /// @brief
    ///   Checks if this is a null depth stencil view.
    ///
    /// @return bool
    /// @retval true   This is a null depth stencil view.
    /// @retval false  This is not a null depth stencil view.
    YAGE_NODISCARD auto IsNull() const noexcept -> bool { return handle.IsNull(); }

    /// @brief
    ///   Get CPU descriptor handle of this depth stencil view.
    ///
    /// @return CpuDescriptorHandle
    ///   Return CPU descriptor handle of this depth stencil view.
    YAGE_NODISCARD auto DescriptorHandle() const noexcept -> CpuDescriptorHandle { return handle; }

    /// @brief
    ///   Allow implicit conversion to CPU descriptor handle.
    operator CpuDescriptorHandle() const noexcept { return handle; }

private:
    /// @brief  Descriptor handle of this depth stencil view.
    CpuDescriptorHandle handle;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_DESCRIPTOR_H
