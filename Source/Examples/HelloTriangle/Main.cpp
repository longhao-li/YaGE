#include <YaGE/Core/Exception.h>
#include <YaGE/Core/Window.h>
#include <YaGE/Graphics/CommandBuffer.h>
#include <YaGE/Graphics/SwapChain.h>

#include <memory>

using namespace YaGE;

static auto LoadBinary(const String &path) -> std::vector<uint8_t> {
    HANDLE fileHandle = CreateFileW(reinterpret_cast<LPCWSTR>(path.Data()), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE)
        throw SystemErrorException(GetLastError());

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(fileHandle, &fileSize)) {
        CloseHandle(fileHandle);
        throw SystemErrorException(GetLastError());
    }

    std::vector<uint8_t> buffer(fileSize.QuadPart);
    ReadFile(fileHandle, buffer.data(), static_cast<DWORD>(fileSize.QuadPart), nullptr, nullptr);
    CloseHandle(fileHandle);

    return buffer;
}

struct Vertex {
    float position[4];
    Color color;
};

class Application {
public:
    Application()
        : mainWindow(u"HelloTriangle", 800, 600),
          swapChain(mainWindow),
          rootSignature(),
          pipelineState(),
          commandBuffer(),
          vertexBuffer(3, static_cast<uint32_t>(sizeof(Vertex))) {
        UploadGpuBufferData();
        CreateRootSignature();
        CreateGraphicsPipelineState();

        swapChain.SetClearColor(Color::Azure());
    }

    auto Run() -> void;
    auto Update() -> void;

private:
    auto UploadGpuBufferData() -> void;
    auto CreateRootSignature() -> void;
    auto CreateGraphicsPipelineState() -> void;

private:
    Window    mainWindow;
    SwapChain swapChain;

    std::unique_ptr<RootSignature>         rootSignature;
    std::unique_ptr<GraphicsPipelineState> pipelineState;

    CommandBuffer    commandBuffer;
    StructuredBuffer vertexBuffer;
};

auto Application::UploadGpuBufferData() -> void {
    const Vertex vertices[] = {
        {{0.0f, 0.5f, 0.0f, 1.0f}, Color::Red()},
        {{0.5f, -0.5f, 0.0f, 1.0f}, Color::Green()},
        {{-0.5f, -0.5f, 0.0f, 1.0f}, Color::Blue()},
    };

    commandBuffer.Transition(vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST);
    commandBuffer.CopyBuffer(vertices, vertexBuffer, 0, sizeof(vertices));
    commandBuffer.Transition(vertexBuffer, D3D12_RESOURCE_STATE_GENERIC_READ);
    commandBuffer.Submit();
}

auto Application::CreateRootSignature() -> void {
    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    rootSignature = std::make_unique<RootSignature>(desc);
}

auto Application::CreateGraphicsPipelineState() -> void {
    std::vector<uint8_t> vertexShader(LoadBinary(u"Shaders/HelloTriangle.vso"));
    std::vector<uint8_t> pixelShader(LoadBinary(u"Shaders/HelloTriangle.pso"));

    D3D12_INPUT_ELEMENT_DESC inputElements[]{
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};

    desc.pRootSignature                        = rootSignature->D3D12RootSignature();
    desc.VS.BytecodeLength                     = vertexShader.size();
    desc.VS.pShaderBytecode                    = vertexShader.data();
    desc.PS.BytecodeLength                     = pixelShader.size();
    desc.PS.pShaderBytecode                    = pixelShader.data();
    desc.InputLayout.pInputElementDescs        = inputElements;
    desc.InputLayout.NumElements               = _countof(inputElements);
    desc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
    desc.RasterizerState.CullMode              = D3D12_CULL_MODE_NONE;
    desc.RasterizerState.FrontCounterClockwise = FALSE;
    desc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
    desc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    desc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    desc.RasterizerState.DepthClipEnable       = TRUE;
    desc.RasterizerState.MultisampleEnable     = FALSE;
    desc.RasterizerState.AntialiasedLineEnable = FALSE;
    desc.RasterizerState.ForcedSampleCount     = 0;
    desc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    desc.BlendState.AlphaToCoverageEnable      = FALSE;
    desc.BlendState.IndependentBlendEnable     = FALSE;

    for (auto &renderTarget : desc.BlendState.RenderTarget)
        renderTarget = {
            FALSE,
            FALSE,
            D3D12_BLEND_ONE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL,
        };

    desc.DepthStencilState.DepthEnable   = FALSE;
    desc.DepthStencilState.StencilEnable = FALSE;
    desc.SampleMask                      = UINT_MAX;
    desc.PrimitiveTopologyType           = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets                = 1;
    desc.RTVFormats[0]                   = swapChain.PixelFormat();
    desc.SampleDesc.Count                = 1;

    pipelineState = std::make_unique<GraphicsPipelineState>(*rootSignature, desc);
}

auto Application::Run() -> void {
    MSG msg{};
    while (!mainWindow.IsClosed()) {
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
    commandBuffer.SetRenderTarget(backBuffer);
    commandBuffer.ClearColor(backBuffer);

    commandBuffer.SetPipelineState(*pipelineState);
    commandBuffer.SetGraphicsRootSignature(*rootSignature);

    commandBuffer.SetViewport(0, 0, mainWindow.ClientWidth(), mainWindow.ClientHeight());
    commandBuffer.SetScissorRect(0, 0, mainWindow.ClientWidth(), mainWindow.ClientHeight());

    commandBuffer.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandBuffer.SetVertexBuffer(0, vertexBuffer);

    commandBuffer.Draw(3);

    commandBuffer.Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandBuffer.Submit();

    swapChain.Present();
}

int WINAPI WinMain(_In_ HINSTANCE     hInstance,
                   _In_opt_ HINSTANCE hPrevInstance,
                   _In_ LPSTR         lpCmdLine,
                   _In_ int           nShowCmd) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nShowCmd;

    try {
        auto app = std::make_unique<Application>();
        app->Run();
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
