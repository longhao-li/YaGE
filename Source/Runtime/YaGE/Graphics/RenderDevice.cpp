#include "RenderDevice.h"
#include "../Core/Exception.h"

using namespace YaGE;

#if defined(_DEBUG) || !defined(NDEBUG)
#    define YART_DEBUG_BUILD true
#else
#    define YART_DEBUG_BUILD false
#endif

YaGE::RenderDevice::RenderDevice()
    : dxgiFactory(),
      adapter(),
      device(),
      commandQueue(),
      fence(),
      nextFenceValue(1),
      allocatorPool(),
      allocatorPoolMutex(),
      freeAllocatorQueue(),
      freeAllocatorQueueMutex(),
      constantBufferViewAllocator(),
      samplerViewAllocator(),
      renderTargetViewAllocator(),
      depthStencilViewAllocator() {
    HRESULT hr = S_OK;

#if defined(_DEBUG) || !defined(NDEBUG)
    { // Enable D3D12 debug layer.
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        hr = D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()));
        if (SUCCEEDED(hr))
            debugController->EnableDebugLayer();
    }
#endif

    // Create DXGI factory.
    hr = CreateDXGIFactory2(YART_DEBUG_BUILD ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create DXGI factory.");

    // Enum adapters and create device.
    for (UINT i = 0;; ++i) {
        hr = dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                     IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()));
        if (hr == DXGI_ERROR_NOT_FOUND)
            break;

        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to enum adapters.");

        // Try to create D3D12 device.
        hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(device.GetAddressOf()));
        if (SUCCEEDED(hr))
            break;
    }

    if (device == nullptr)
        throw RenderAPIException(hr, u"No suitable GPU found.");

    { // Create CommandQueue.
        const D3D12_COMMAND_QUEUE_DESC desc{
            /* Type     = */ D3D12_COMMAND_LIST_TYPE_DIRECT,
            /* Priority = */ D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
            /* Flags    = */ D3D12_COMMAND_QUEUE_FLAG_NONE,
            /* NodeMask = */ 0,
        };

        hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to create command queue.");
    }

    // Create fence.
    hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create fence.");

    // Initialize descriptor allocators.
    constantBufferViewAllocator.Initialize(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    samplerViewAllocator.Initialize(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    renderTargetViewAllocator.Initialize(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    depthStencilViewAllocator.Initialize(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

YaGE::RenderDevice::~RenderDevice() noexcept { Sync(); }

auto YaGE::RenderDevice::Sync(uint64_t syncPoint) const noexcept -> void {
    if (IsSyncPointReached(syncPoint))
        return;

    struct FenceEvent {
        HANDLE handle;

        FenceEvent() noexcept : handle(CreateEvent(nullptr, FALSE, FALSE, nullptr)) {}
        ~FenceEvent() noexcept { CloseHandle(handle); }
    };

    static thread_local FenceEvent fenceEvent;

    const HANDLE event = fenceEvent.handle;
    fence->SetEventOnCompletion(syncPoint, event);
    WaitForSingleObject(event, INFINITE);
}

YAGE_NODISCARD auto YaGE::RenderDevice::AcquireCommandAllocator() -> ID3D12CommandAllocator * {
    { // Try to get one from free allocator queue.
        ID3D12CommandAllocator *allocator = nullptr;
        { // Lock scope.
            std::lock_guard<std::mutex> lock(freeAllocatorQueueMutex);
            if (!freeAllocatorQueue.empty()) {
                auto &front = freeAllocatorQueue.front();
                if (IsSyncPointReached(front.first)) {
                    allocator = front.second;
                    freeAllocatorQueue.pop();
                }
            }
        }

        if (allocator != nullptr) {
            allocator->Reset();
            return allocator;
        }
    }

    // Try to create a new command allocaator.
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.GetAddressOf()));
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create command allocator.");

    // Add to allocator pool.
    ID3D12CommandAllocator     *result = allocator.Get();
    std::lock_guard<std::mutex> lock(allocatorPoolMutex);
    allocatorPool.push(std::move(allocator));

    return result;
}

auto YaGE::RenderDevice::FreeCommandAllocator(uint64_t syncPoint, ID3D12CommandAllocator *allocator) noexcept -> void {
    std::lock_guard<std::mutex> lock(freeAllocatorQueueMutex);
    freeAllocatorQueue.emplace(syncPoint, allocator);
}

YAGE_NODISCARD auto YaGE::RenderDevice::SupportRayTracing() const noexcept -> bool {
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 feature{};

    HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &feature, sizeof(feature));
    if (FAILED(hr))
        return false;

    return feature.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

YAGE_NODISCARD auto YaGE::RenderDevice::SupportUnorderedAccess(DXGI_FORMAT format) const noexcept -> bool {
    switch (format) {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SINT:
        return true;

    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B4G4R4A4_UNORM: {
        HRESULT                          hr = S_OK;
        D3D12_FEATURE_DATA_D3D12_OPTIONS feature{};

        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &feature, static_cast<UINT>(sizeof(feature)));
        if (SUCCEEDED(hr)) {
            if (feature.TypedUAVLoadAdditionalFormats) {
                D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport{};
                formatSupport.Format = format;

                hr = device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport,
                                                 static_cast<UINT>(sizeof(formatSupport)));
                if (SUCCEEDED(hr)) {
                    if ((formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) == 0)
                        return false;

                    if ((formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE) == 0)
                        return false;

                    return true;
                }
            }
        }

        return false;
    }

    default:
        return false;
    }
}

YAGE_NODISCARD auto YaGE::RenderDevice::Singleton() -> RenderDevice & {
    static RenderDevice instance;
    return instance;
}
