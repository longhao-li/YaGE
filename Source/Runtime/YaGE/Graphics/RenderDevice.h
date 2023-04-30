#ifndef YAGE_GRAPHICS_RENDER_DEVICE_H
#define YAGE_GRAPHICS_RENDER_DEVICE_H

#include "Descriptor.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>

namespace YaGE {

class RenderDevice {
public:
    /// @brief
    ///   Create and initialize a RenderDevice object.
    ///
    /// @throw RenderAPIException
    ///   Thrown if failed to create D3D12 device.
    YAGE_API RenderDevice();

    /// @brief
    ///   Destroy this RenderDevice object.
    YAGE_API ~RenderDevice() noexcept;

    /// @brief
    ///   Get DXGI factory that is used to create this RenderDevice.
    ///
    /// @return IDXGIFactory6 *
    ///   Return the DXGI factory that is used to create this RenderDevice.
    YAGE_NODISCARD auto DXGIFactory() const noexcept -> IDXGIFactory6 * { return dxgiFactory.Get(); }

    /// @brief
    ///   Get DXGI adapter that is used to create this RenderDevice.
    ///
    /// @return IDXGIAdapter3 *
    ///   Return the DXGI adapter that is used to create this RenderDevice.
    YAGE_NODISCARD auto Adapter() const noexcept -> IDXGIAdapter1 * { return adapter.Get(); }

    /// @brief
    ///   Get D3D12 device of this RenderDevice.
    ///
    /// @return ID3D12Device5 *
    ///   Return D3D12 device of this RenderDevice.
    YAGE_NODISCARD auto Device() const noexcept -> ID3D12Device1 * { return device.Get(); }

    /// @brief
    ///   Get direct command queue of this RenderDevice.
    ///
    /// @return ID3D12CommandQueue *
    ///   Return direct command queue of this RenderDevice.
    YAGE_NODISCARD auto CommandQueue() const noexcept -> ID3D12CommandQueue * { return commandQueue.Get(); }

    /// @brief
    ///   Signal and increment sync point for this RenderDevice. This value could be used for CPU-GPU sync.
    ///
    /// @return uint64_t
    ///   Return the signaled sync point value.
    YAGE_NODISCARD auto AcquireSyncPoint() const noexcept -> uint64_t {
        const uint64_t value = nextFenceValue.fetch_add(1, std::memory_order_relaxed);
        commandQueue->Signal(fence.Get(), value);
        return value;
    }

    /// @brief
    ///   Checks if the specified sync point is reached by GPU.
    ///
    /// @param syncPoint    The sync point to be checked.
    ///
    /// @return bool
    /// @retval true    The specified sync point has been reached.
    /// @retval false   The specified sync point has not been reached.
    YAGE_NODISCARD auto IsSyncPointReached(uint64_t syncPoint) const noexcept -> bool {
        return syncPoint <= fence->GetCompletedValue();
    }

    /// @brief
    ///   Wait for the specified sync point to be reached. This method will block current thread until the specified sync point is reached.
    ///
    /// @param syncPoint    The fence value to be waited for.
    YAGE_API auto Sync(uint64_t syncPoint) const noexcept -> void;

    /// @brief
    ///   Wait for all tasks in command queue to be finished. This method will block current thread until all tasks in command queue are finished.
    auto Sync() const noexcept -> void { Sync(AcquireSyncPoint()); }

    /// @brief
    ///   Try to acquire a new direct command allocator from this RenderDevice.
    /// @remarks
    ///   This method will try to acquire a new direct command allocator from this RenderDevice. If there is no available command allocator in the pool, a new command allocator will be created and returned.
    ///
    /// @return ID3D12CommandAllocator *
    ///   Return a new direct command allocator.
    /// @throw RenderAPIException
    ///   Thrown if failed to create new command allocator.
    YAGE_NODISCARD YAGE_API auto AcquireCommandAllocator() -> ID3D12CommandAllocator *;

    /// @brief
    ///   Free a direct command allocator.
    ///
    /// @param syncPoint        The fence value that is signaled when the command allocator is no longer in use.
    /// @param allocator[in]    The command allocator to be freed.
    YAGE_API auto FreeCommandAllocator(uint64_t syncPoint, ID3D12CommandAllocator *allocator) noexcept -> void;

    /// @brief
    ///   Allocate a constant buffer view descriptor.
    /// @note
    ///   The returned descriptor is not initialized.
    ///
    /// @return CpuDescriptorHandle
    ///   Return a new constant buffer view descriptor.
    /// @throw RenderAPIException
    ///   Thrown if descriptor allocator failed to create new descriptor heap.
    YAGE_NODISCARD auto AllocateConstantBufferView() -> CpuDescriptorHandle {
        return constantBufferViewAllocator.Allocate();
    }

    /// @brief
    ///   Free a constant buffer view descriptor.
    ///
    /// @param handle   The descriptor to be freed.
    auto FreeConstantBufferView(CpuDescriptorHandle handle) noexcept -> void {
        constantBufferViewAllocator.Free(handle);
    }

    /// @brief
    ///   Allocate a shader resource view descriptor.
    /// @note
    ///   The returned descriptor is not initialized.
    ///
    /// @return CpuDescriptorHandle
    ///   Return a new shader resource view descriptor.
    /// @throw RenderAPIException
    ///   Thrown if descriptor allocator failed to create new descriptor heap.
    YAGE_NODISCARD auto AllocateShaderResourceView() -> CpuDescriptorHandle {
        return constantBufferViewAllocator.Allocate();
    }

    /// @brief
    ///   Free a shader resource view descriptor.
    ///
    /// @param handle   The descriptor to be freed.
    auto FreeShaderResourceView(CpuDescriptorHandle handle) noexcept -> void {
        constantBufferViewAllocator.Free(handle);
    }

    /// @brief
    ///   Allocate an unordered access view descriptor.
    /// @note
    ///   The returned descriptor is not initialized.
    ///
    /// @return CpuDescriptorHandle.
    ///   Return a new unordered access view descriptor.
    /// @throw RenderAPIException
    ///   Thrown if descriptor allocator failed to create new descriptor heap.
    YAGE_NODISCARD auto AllocateUnorderedAccessView() -> CpuDescriptorHandle {
        return constantBufferViewAllocator.Allocate();
    }

    /// @brief
    ///   Free an unordered access view descriptor.
    ///
    /// @param handle   The descriptor to be freed.
    auto FreeUnorderedAccessView(CpuDescriptorHandle handle) noexcept -> void {
        constantBufferViewAllocator.Free(handle);
    }

    /// @brief
    ///   Allocate a sampler view descriptor.
    /// @note
    ///   The returned descriptor is not initialized.
    ///
    /// @return CpuDescriptorHandle
    ///   Return a new sampler view descriptor.
    /// @throw RenderAPIException
    ///   Thrown if descriptor allocator failed to create new descriptor heap.
    YAGE_NODISCARD auto AllocateSamplerView() -> CpuDescriptorHandle { return samplerViewAllocator.Allocate(); }

    /// @brief
    ///   Free a sampler view descriptor.
    ///
    /// @param handle   The descriptor to be freed.
    auto FreeSamplerView(CpuDescriptorHandle handle) noexcept -> void { samplerViewAllocator.Free(handle); }

    /// @brief
    ///   Allocate a render target view descriptor.
    /// @note
    ///   The returned descriptor is not initialized.
    ///
    /// @return CpuDescriptorHandle
    ///   Return a new render target view descriptor.
    /// @throw RenderAPIException
    ///   Thrown if descriptor allocator failed to create new descriptor heap.
    YAGE_NODISCARD auto AllocateRenderTargetView() -> CpuDescriptorHandle {
        return renderTargetViewAllocator.Allocate();
    }

    /// @brief
    ///   Free a render target view descriptor.
    ///
    /// @param handle   The descriptor to be freed.
    auto FreeRenderTargetView(CpuDescriptorHandle handle) noexcept -> void { renderTargetViewAllocator.Free(handle); }

    /// @brief
    ///   Allocate a depth stencil view descriptor.
    /// @note
    ///   The returned descriptor is not initialized.
    ///
    /// @return CpuDescriptorHandle
    ///   Return a new depth stencil view descriptor.
    /// @throw RenderAPIException
    ///   Thrown if descriptor allocator failed to create new descriptor heap.
    YAGE_NODISCARD auto AllocateDepthStencilView() -> CpuDescriptorHandle {
        return depthStencilViewAllocator.Allocate();
    }

    /// @brief
    ///   Free a depth stencil view descriptor.
    ///
    /// @param handle   The descriptor to be freed.
    auto FreeDepthStencilView(CpuDescriptorHandle handle) noexcept -> void { depthStencilViewAllocator.Free(handle); }

    /// @brief
    ///   Checks if this RenderDevice supports DirectX ray tracing.
    ///
    /// @return bool
    /// @retval true    This RenderDevice supports DirectX ray tracing.
    /// @retval false   This RenderDevice does not support DirectX ray tracing.
    YAGE_NODISCARD YAGE_API auto SupportRayTracing() const noexcept -> bool;

    /// @brief
    ///   Checks if the specified pixel format is supported for unordered access.
    ///
    /// @param format   The pixel format to be checked.
    ///
    /// @return bool
    /// @retval true    The specified pixel format is supported for unordered access.
    /// @retval false   The specified pixel format is not supported for unordered access.
    YAGE_NODISCARD YAGE_API auto SupportUnorderedAccess(DXGI_FORMAT format) const noexcept -> bool;

    /// @brief
    ///   Get global singleton instance of RenderDevice.
    ///
    /// @return RenderDevice &
    ///   Return reference to RenderDevice global singleton instance.
    /// @throw RenderAPIException
    ///   Thrown if failed to create the RenderDevice singleton instance.
    YAGE_NODISCARD YAGE_API static auto Singleton() -> RenderDevice &;

private:
    /// @brief  DXGI factory object that is used to create D3D12 objects.
    Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;

    /// @brief  The adapter that is used to create D3D12 device.
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

    /// @brief  D3D12 virtual device object.
    Microsoft::WRL::ComPtr<ID3D12Device1> device;

    /// @brief  Direct command queue for this RenderDevice.
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

    /// @brief  Fence object that is used to synchronize CPU and GPU for this device.
    Microsoft::WRL::ComPtr<ID3D12Fence1> fence;

    /// @brief  Next fence value to be signaled.
    mutable std::atomic_uint64_t nextFenceValue;

    /// @brief  D3D12 command allocator pool.
    std::stack<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> allocatorPool;

    /// @brief  Allocator that is used to protect D3D12 command allocator pool.
    mutable std::mutex allocatorPoolMutex;

    /// @brief  Freed D3D12 command allocator queue to be reused.
    std::queue<std::pair<uint64_t, ID3D12CommandAllocator *>> freeAllocatorQueue;

    /// @brief  Mutex that is used to protect free command allocator queue.
    mutable std::mutex freeAllocatorQueueMutex;

    /// @brief  CBV/SRV/UAV allocator for this device.
    CpuDescriptorAllocator constantBufferViewAllocator;

    /// @brief  Sampler view allocator for this device.
    CpuDescriptorAllocator samplerViewAllocator;

    /// @brief  RTV allocator for this device.
    CpuDescriptorAllocator renderTargetViewAllocator;

    /// @brief  DSV allocator for this device.
    CpuDescriptorAllocator depthStencilViewAllocator;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_RENDER_DEVICE_H
