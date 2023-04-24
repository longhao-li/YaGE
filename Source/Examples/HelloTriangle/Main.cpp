#include <YaGE/Core/Exception.h>
#include <YaGE/Core/Window.h>
#include <YaGE/Graphics/CommandBuffer.h>
#include <YaGE/Graphics/SwapChain.h>

#include <fmt/chrono.h>

#include <chrono>

using namespace YaGE;

int WINAPI WinMain(_In_ HINSTANCE     hInstance,
                   _In_opt_ HINSTANCE hPrevInstance,
                   _In_ LPSTR         lpCmdLine,
                   _In_ int           nShowCmd) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nShowCmd;

    LARGE_INTEGER now, lastUpdate, lastUpdateFps;
    QueryPerformanceCounter(&now);
    lastUpdate    = now;
    lastUpdateFps = now;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    const double invFreq = 1.0 / static_cast<double>(frequency.QuadPart);

    try {
        Window window(u"Hello Triangle", 800, 600, WindowStyle::Default);
        window.Center();

        SwapChain     swapChain(window, 3, DXGI_FORMAT_R8G8B8A8_UNORM, true);
        CommandBuffer commandBuffer;

        swapChain.SetClearColor(Color::Azure());

        MSG msg{};
        while (!window.IsClosed()) {
            if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            } else {
                auto &backBuffer = swapChain.CurrentBackBuffer();

                commandBuffer.SetRenderTarget(backBuffer);
                commandBuffer.ClearColor(backBuffer);
                commandBuffer.Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
                commandBuffer.Submit();

                swapChain.Present();

                QueryPerformanceCounter(&now);
                const double deltaTime = static_cast<double>(now.QuadPart - lastUpdate.QuadPart) * invFreq;

                if (now.QuadPart - lastUpdateFps.QuadPart >= frequency.QuadPart) {
                    String newTitle(Format(u"Hello Triangle - {:.2f} FPS", 1.0 / deltaTime));
                    window.SetTitle(newTitle);
                    lastUpdateFps = now;
                }

                lastUpdate = now;
            }
        }
    } catch (YaGE::SystemErrorException &e) {
        YaGE::String error = YaGE::Format(u"{} Error code: 0x{:X}.", e.Message(), uint32_t(e.ErrorCode()));
        MessageBoxW(nullptr, reinterpret_cast<LPCWSTR>(error.Data()), L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
