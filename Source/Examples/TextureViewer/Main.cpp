#include <YaGE/Core/Exception.h>
#include <YaGE/Core/Window.h>
#include <YaGE/Graphics/CommandBuffer.h>
#include <YaGE/Graphics/SwapChain.h>
#include <YaGE/Resource/Image.h>

#include <iostream>

using namespace YaGE;

static auto ToString(const char *str) noexcept -> String {
    size_t len   = strlen(str);
    int    count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, static_cast<int>(len), nullptr, 0);
    if (count <= 0)
        return {};

    String result;
    result.Resize(static_cast<size_t>(count));
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, static_cast<int>(len), reinterpret_cast<LPWSTR>(result.Data()),
                        count);

    return result;
}

class Application {
public:
    Application(const String &path)
        : texture(path),
          mainWindow(u"Texture Viewer", texture.Width(), texture.Height()),
          swapChain(mainWindow, 2, texture.PixelFormat()),
          commandBuffer() {
        swapChain.SetClearColor(Color::Black());
    }

    auto Run() -> void;
    auto Update() -> void;

private:
    Image         texture;
    Window        mainWindow;
    SwapChain     swapChain;
    CommandBuffer commandBuffer;
};

auto Application::Run() -> void {
    MSG msg{};
    while (!mainWindow.IsClosed() && msg.message != WM_QUIT) {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            Update();
        }
    }
}

auto Application::Update() -> void {
    auto &backBuffer = swapChain.CurrentBackBuffer();

    commandBuffer.Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandBuffer.ClearColor(backBuffer);
    commandBuffer.CopyTexture(texture.Width(), texture.Height(), texture.PixelFormat(), texture.Data(),
                              texture.RowPitch(), backBuffer, 0);
    commandBuffer.Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandBuffer.Submit();

    swapChain.Present();
}

auto main(int argc, char **argv) -> int {
    if (argc < 2) {
        std::cout << "Usage: TextureViewer <image file>" << std::endl;
        return 0;
    }

    try {
        Application app(ToString(argv[1]));
        app.Run();
    } catch (const YaGE::SystemErrorException &e) {
        YaGE::String error = YaGE::Format(u"{} Error code: 0x{:X}.", e.Message(), uint32_t(e.ErrorCode()));
        MessageBoxW(nullptr, reinterpret_cast<LPCWSTR>(error.Data()), L"Error", MB_OK | MB_ICONERROR);
        return 1;
    } catch (const YaGE::Exception &e) {
        MessageBoxW(nullptr, reinterpret_cast<LPCWSTR>(e.Message().Data()), L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
