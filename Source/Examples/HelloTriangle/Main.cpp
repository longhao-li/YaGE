#include <YaGE/Core/Exception.h>
#include <YaGE/Core/Window.h>

#include <fmt/chrono.h>

#include <chrono>

using namespace YaGE;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nShowCmd;
    
    try {
        YaGE::Window window(u"Hello Triangle", 800, 600, YaGE::WindowStyle::Overlapped);
        window.Center();

        MSG msg{};
        while (!window.IsClosed()) {
            if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            } else {
                String newTitle(Format(u"Hello Triangle - {}", std::chrono::system_clock::now()));
                if (newTitle != window.Title())
                    window.SetTitle(newTitle);
            }
        }
    } catch (YaGE::SystemErrorException &e) {
        YaGE::String error = YaGE::Format(u"{} Error code: 0x{:X}.", e.Message(), uint32_t(e.ErrorCode()));
        MessageBoxW(nullptr, reinterpret_cast<LPCWSTR>(error.Data()), L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
