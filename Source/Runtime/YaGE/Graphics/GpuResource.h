#ifndef YAGE_GRAPHICS_GPU_RESOURCE_H
#define YAGE_GRAPHICS_GPU_RESOURCE_H

#include "../Core/Common.h"

#include <d3d12.h>
#include <wrl/client.h>

#include <utility>

namespace YaGE {

class GpuResource {
protected:
    /// @brief
    ///   Create an empty GPU resource.
    GpuResource() noexcept : resource(nullptr), usageState(D3D12_RESOURCE_STATE_COMMON) {}

    /// @brief
    ///   Copy constructor is disabled.
    GpuResource(const GpuResource &) = delete;

    /// @brief
    ///   Copy assignment is disabled.
    auto operator=(const GpuResource &) = delete;

    /// @brief
    ///   Move constructor. The moved GpuResource will be invalidated.
    ///
    /// @param other    The GpuResource to be moved.
    GpuResource(GpuResource &&other) noexcept : resource(std::move(other.resource)), usageState(other.usageState) {
        other.resource   = nullptr;
        other.usageState = D3D12_RESOURCE_STATE_COMMON;
    }

    /// @brief
    ///   Move assignment. The moved GpuResource will be invalidated.
    ///
    /// @param other    The GpuResource to be moved.
    auto operator=(GpuResource &&other) noexcept -> GpuResource & {
        resource   = std::move(other.resource);
        usageState = other.usageState;

        other.resource   = nullptr;
        other.usageState = D3D12_RESOURCE_STATE_COMMON;

        return *this;
    }

public:
    /// @brief
    ///   Destroy this GPU resource.
    YAGE_API virtual ~GpuResource() noexcept;

    /// @brief
    ///   Get current state of this GPU resource.
    ///
    /// @return D3D12_RESOURCE_STATUS
    ///   Return current state of this GPU resource.
    YAGE_NODISCARD auto State() const noexcept -> D3D12_RESOURCE_STATES { return usageState; }

    friend class CommandBuffer;

protected:
    /// @brief  D3D12 resource handle.
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;

    /// @brief  D3D12 resource state.
    D3D12_RESOURCE_STATES usageState;
};

} // namespace YaGE

#endif // YAGE_GRAPHICS_GPU_RESOURCE_H
