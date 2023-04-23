#include "SwapChain.h"
#include "../Core/Exception.h"

using namespace YaGE;

YaGE::SwapChain::SwapChain(HWND window, uint32_t numBuffers, DXGI_FORMAT bufferFormat, bool enableTearing)
    : renderDevice(RenderDevice::Singleton()),
      swapChain(),
      tearingEnabled(false),
      bufferCount(numBuffers > 2 ? 3 : 2),
      bufferIndex(0),
      pixelFormat(bufferFormat),
      backBuffers(),
      presentSyncPoints() {
    IDXGIFactory6 *const dxgiFactory = renderDevice.DXGIFactory();

    // Query tearing support.
    if (enableTearing) {
        BOOL tearingSupport = FALSE;
        if (SUCCEEDED(dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tearingSupport,
                                                       sizeof(tearingSupport))))
            tearingEnabled = (tearingSupport == TRUE);
    }

    // Create swap chain.
    RECT rect;
    GetClientRect(window, &rect);

    UINT flags = 0;
    if (tearingEnabled)
        flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    const DXGI_SWAP_CHAIN_DESC1 desc{
        /* Width      = */ static_cast<UINT>(rect.right),
        /* Height     = */ static_cast<UINT>(rect.bottom),
        /* Format     = */ pixelFormat,
        /* Stereo     = */ FALSE,
        /* SampleDesc = */
        {
            /* Count   = */ 1,
            /* Quality = */ 0,
        },
        /* BufferUsage = */ DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER,
        /* BufferCount = */ bufferCount,
        /* Scaling     = */ DXGI_SCALING_NONE,
        /* SwapEffect  = */ DXGI_SWAP_EFFECT_FLIP_DISCARD,
        /* AlphaMode   = */ DXGI_ALPHA_MODE_UNSPECIFIED,
        /* Flags       = */ flags,
    };

    HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(renderDevice.CommandQueue(), window, &desc, nullptr, nullptr,
                                                     swapChain.GetAddressOf());
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to create swap chain.");

    // Disable Alt+Enter.
    dxgiFactory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);

    // Get back buffers.
    for (uint32_t i = 0; i < bufferCount; ++i) {
        Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
        hr = swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to get swap chain back buffer.");

        backBuffers[i].ResetSwapChainResource(std::move(backBuffer));
    }
}

YaGE::SwapChain::~SwapChain() noexcept { renderDevice.Sync(); }

auto YaGE::SwapChain::Present() noexcept -> uint64_t {
    swapChain->Present(0, tearingEnabled ? DXGI_PRESENT_ALLOW_TEARING : 0);

    // Acquire a sync point.
    const uint64_t syncPoint       = renderDevice.AcquireSyncPoint();
    presentSyncPoints[bufferIndex] = syncPoint;

    // Increase buffer index.
    bufferIndex = (bufferIndex + 1) % bufferCount;

    return syncPoint;
}

auto YaGE::SwapChain::Resize(uint32_t width, uint32_t height) -> void {
    // Release resources.
    renderDevice.Sync();
    for (uint32_t i = 0; i < bufferCount; ++i)
        backBuffers[i].ReleaseSwapChainResource();

    // Resize back buffers.
    UINT flags = 0;
    if (tearingEnabled)
        flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    HRESULT hr = swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, flags);
    if (FAILED(hr))
        throw RenderAPIException(hr, u"Failed to resize back buffers.");

    // Get back buffers.
    for (uint32_t i = 0; i < bufferCount; ++i) {
        Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
        hr = swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));
        if (FAILED(hr))
            throw RenderAPIException(hr, u"Failed to get swap chain back buffer.");
    }

    bufferIndex = 0;
}
