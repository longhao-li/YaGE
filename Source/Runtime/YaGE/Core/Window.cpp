#include "Window.h"
#include "Exception.h"

#include <shellapi.h>
#include <shellscalingapi.h>
#include <windowsx.h>

using namespace YaGE;

namespace {

static LRESULT CALLBACK WindowInitializeProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept {
    if (uMsg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCT *>(lParam);
        SetWindowLongPtrW(hWnd, 0, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static auto SupportWindows10BuildFeatures(WORD build) noexcept {
    OSVERSIONINFOEXW info{
        /* dwOSVersionInfoSize = */ sizeof(OSVERSIONINFOEX),
        /* dwMajorVersion      = */ 10,
        /* dwMinorVersion      = */ 0,
        /* dwBuildNumber       = */ build,
        /* dwPlatformId        = */ 0,
        /* szCSDVersion        = */ {},
        /* wServicePackMajor   = */ 0,
        /* wServicePackMinor   = */ 0,
        /* wSuiteMask          = */ 0,
        /* wProductType        = */ 0,
        /* wReserved           = */ 0,
    };

    DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER;

    DWORDLONG condition = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    condition           = VerSetConditionMask(condition, VER_MINORVERSION, VER_GREATER_EQUAL);
    condition           = VerSetConditionMask(condition, VER_BUILDNUMBER, VER_GREATER_EQUAL);

    return VerifyVersionInfoW(&info, mask, condition) != FALSE;
}

class WindowClass {
public:
    WindowClass() : hInstance(GetModuleHandleW(nullptr)) {
        const WNDCLASSEXW clsEx{
            /* cbSize        = */ sizeof(WNDCLASSEX),
            /* style         = */ CS_HREDRAW | CS_VREDRAW,
            /* lpfnWndProc   = */ WindowInitializeProc,
            /* cbClsExtra    = */ 0,
            /* cbWndExtra    = */ sizeof(void *),
            /* hInstance     = */ hInstance,
            /* hIcon         = */ nullptr,
            /* hCursor       = */ LoadCursor(nullptr, IDC_ARROW),
            /* hbrBackground = */ nullptr,
            /* lpszMenuName  = */ nullptr,
            /* lpszClassName = */ L"YaGE",
            /* hIconSm       = */ nullptr,
        };

        classId = RegisterClassExW(&clsEx);
        if (classId == 0)
            throw SystemErrorException(GetLastError(), u"Failed to register win32 class.");
    }

    /// @brief
    ///   Copy constructor is disabled.
    WindowClass(const WindowClass &) = delete;

    /// @brief
    ///   Copy assignment operator is disabled.
    auto operator=(const WindowClass &) = delete;

    /// @brief
    ///   Unregister this window class.
    ~WindowClass() noexcept { UnregisterClassW(reinterpret_cast<LPCWSTR>(classId), hInstance); }

    /// @brief
    ///   Allow implicit conversion to LPCWSTR.
    operator LPCWSTR() const noexcept { return reinterpret_cast<LPCWSTR>(classId); }

    /// @brief
    ///   Get window class singleton instance.
    ///
    /// @return WindowClass &
    ///   Return window class singleton instance.
    /// @throw SystemErrorException
    ///   Thrown if failed to register win32 class.
    YAGE_NODISCARD static auto Singleton() -> WindowClass & {
        static WindowClass instance;
        return instance;
    }

private:
    /// @brief  Instance handle.
    HINSTANCE hInstance;

    /// @brief  Class ID.
    ATOM classId;
};

static auto GetHWNDStyle(WindowStyle flags, DWORD &dwStyle, DWORD &dwStyleEx) noexcept -> void {
    dwStyle   = 0;
    dwStyleEx = 0;

    if ((flags & WindowStyle::Titled) != WindowStyle::None) {
        dwStyle |= (WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU);
        dwStyleEx |= WS_EX_APPWINDOW;

        if ((flags & WindowStyle::Minimizable) != WindowStyle::None)
            dwStyle |= WS_MINIMIZEBOX;
        if ((flags & WindowStyle::Maximizable) != WindowStyle::None)
            dwStyle |= WS_MAXIMIZEBOX;
    } else if ((flags & WindowStyle::BorderLess) != WindowStyle::None) {
        dwStyle |= WS_POPUP;
    }

    if ((flags & WindowStyle::Resizable) != WindowStyle::None)
        dwStyle |= WS_SIZEBOX;

    if ((flags & WindowStyle::TopMost) != WindowStyle::None)
        dwStyleEx |= WS_EX_TOPMOST;
    if ((flags & WindowStyle::FileDrop) != WindowStyle::None)
        dwStyleEx |= WS_EX_ACCEPTFILES;
}

class DynamicLibrary {
public:
    /// @brief
    ///   Load a dynamic library.
    ///
    /// @param path Path to the dynamic library.
    DynamicLibrary(LPCTSTR path) : module(LoadLibrary(path)) {
        if (module == nullptr)
            throw SystemErrorException(GetLastError(), u"Failed to load DLL.");
    }

    /// @brief
    ///   Copy constructor is disabled.
    DynamicLibrary(const DynamicLibrary &) = delete;

    /// @brief
    ///   Copy assignment operator is disabled.
    auto operator=(const DynamicLibrary &) = delete;

    /// @brief
    ///   Unload this dynamic library.
    ~DynamicLibrary() noexcept { FreeLibrary(module); }

    /// @brief
    ///   Get function pointer from this dynamic library.
    ///
    /// @param name Function name.
    template <typename T>
    YAGE_NODISCARD auto Load(LPCSTR name) const noexcept -> T {
        return reinterpret_cast<T>(GetProcAddress(module, name));
    }

private:
    /// @brief  Module handle.
    HMODULE module;
};

using PFN_SetProcessDpiAwarenessContext = BOOL (*)(_In_ DPI_AWARENESS_CONTEXT);
using PFN_SetProcessDpiAwareness        = HRESULT (*)(_In_ PROCESS_DPI_AWARENESS);

static auto SetDpiAwareness() noexcept -> void {
    if (SupportWindows10BuildFeatures(15063)) {
        static DynamicLibrary user32(L"User32.dll");
        static auto           func = user32.Load<PFN_SetProcessDpiAwarenessContext>("SetProcessDpiAwarenessContext");
        func(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    } else {
        static DynamicLibrary shcore(L"Shcore.dll");
        static auto           func = shcore.Load<PFN_SetProcessDpiAwareness>("SetProcessDpiAwareness");
        func(PROCESS_PER_MONITOR_DPI_AWARE);
    }
}

static auto GetModifierKeyStatus() noexcept -> ModifierKey {
    ModifierKey mods = ModifierKey::None;

    if (GetKeyState(VK_SHIFT) & 0x8000)
        mods |= ModifierKey::Shift;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        mods |= ModifierKey::Control;
    if (GetKeyState(VK_MENU) & 0x8000)
        mods |= ModifierKey::Alt;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        mods |= ModifierKey::Super;

    return mods;
}

} // namespace

YaGE::Window::Window(StringView title, uint32_t width, uint32_t height, WindowStyle style)
    : hInstance(GetModuleHandleW(nullptr)),
      hWnd(nullptr),
      title(title),
      clientWidth(),
      clientHeight(),
      highSurrogate() {
    // Set DPI awareness.
    SetDpiAwareness();

    // Register class.
    WindowClass &cls = WindowClass::Singleton();

    // Adjust rectangle.
    DWORD dwStyle, dwStyleEx;
    GetHWNDStyle(style, dwStyle, dwStyleEx);

    RECT rect{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    AdjustWindowRectEx(&rect, dwStyle, FALSE, dwStyleEx);

    // Create window.
    hWnd = CreateWindowExW(dwStyleEx, cls, reinterpret_cast<LPCWSTR>(this->title.Data()), dwStyle, CW_USEDEFAULT,
                           CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance,
                           this);
    if (hWnd == nullptr)
        throw SystemErrorException(GetLastError(), u"Failed to create HWND.");

    // Get client size.
    GetClientRect(hWnd, &rect);
    clientWidth  = static_cast<uint32_t>(rect.right - rect.left);
    clientHeight = static_cast<uint32_t>(rect.bottom - rect.top);

    // Show window.
    ShowWindow(hWnd, SW_SHOW);

    // Change window proc function. Separate window proc function is used to avoid calling virtual functions in constructor.
    SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Window::__WindowProc__));
}

YaGE::Window::~Window() noexcept { InternalDestroy(); }

auto YaGE::Window::SetTitle(StringView newTitle) noexcept -> void {
    if (hWnd == nullptr)
        return;

    this->title = newTitle;
    SetWindowTextW(hWnd, reinterpret_cast<LPCWSTR>(this->title.Data()));
}

auto YaGE::Window::Resize(uint32_t width, uint32_t height) noexcept -> void {
    if (hWnd == nullptr)
        return;

    RECT rect{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    AdjustWindowRectEx(&rect, GetWindowLong(hWnd, GWL_STYLE), FALSE, GetWindowLong(hWnd, GWL_EXSTYLE));

    SetWindowPos(hWnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                 SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    this->clientWidth  = width;
    this->clientHeight = height;
}

auto YaGE::Window::Center() noexcept -> void {
    if (hWnd == nullptr)
        return;

    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);

    HMONITOR    monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &monitorInfo);

    RECT screenRect = monitorInfo.rcWork;

    int halfScreenWidth  = (screenRect.right - screenRect.left) / 2;
    int halfScreenHeight = (screenRect.bottom - screenRect.top) / 2;

    int halfWindowWidth  = (windowRect.right - windowRect.left) / 2;
    int halfWindowHeight = (windowRect.bottom - windowRect.top) / 2;

    int x = halfScreenWidth - halfWindowWidth;
    int y = halfScreenHeight - halfWindowHeight;

    SetWindowPos(hWnd, nullptr, x, y, -1, -1, SWP_ASYNCWINDOWPOS | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

auto YaGE::Window::OnFocus(bool focused) -> void { (void)focused; }

auto YaGE::Window::OnClose() -> void {}

auto YaGE::Window::OnChar(char32_t codePoint, ModifierKey mods) -> void {
    (void)codePoint;
    (void)mods;
}

auto YaGE::Window::OnKey(KeyCode key, KeyAction action, ModifierKey mods) -> void {
    (void)key;
    (void)action;
    (void)mods;
}

auto YaGE::Window::OnMouseMove(int32_t x, int32_t y) -> void {
    (void)x;
    (void)y;
}

auto YaGE::Window::OnMouseWheel(int32_t x, int32_t y, float deltaX, float deltaY, ModifierKey mods) -> void {
    (void)x;
    (void)y;
    (void)deltaX;
    (void)deltaY;
    (void)mods;
}

auto YaGE::Window::OnMinimized() -> void {}

auto YaGE::Window::OnMaximized() -> void {}

auto YaGE::Window::OnResize(uint32_t width, uint32_t height) -> void {
    (void)width;
    (void)height;
}

auto YaGE::Window::OnMove(int32_t x, int32_t y) -> void {
    (void)x;
    (void)y;
}

auto YaGE::Window::OnFileDrop(int32_t x, int32_t y, uint32_t count, String paths[]) -> void {
    (void)x;
    (void)y;
    (void)count;
    (void)paths;
}

auto YaGE::Window::InternalDestroy() noexcept -> void {
    if (hWnd == nullptr)
        return;

    DestroyWindow(hWnd);
    hWnd = nullptr;
}

// clang-format off
static constexpr const YaGE::KeyCode KEY_CODE_MAP[0x100] = {
    KeyCode::Undefined,    KeyCode::MouseLeft,      KeyCode::MouseRight, KeyCode::Break,     KeyCode::MouseMiddle,
    KeyCode::MouseX1,      KeyCode::MouseX2,        KeyCode::Undefined,  KeyCode::Backspace, KeyCode::Tab,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Clear,      KeyCode::Enter,     KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Pause,
    KeyCode::CapsLock,     KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Escape,     KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Space,      KeyCode::PageUp,    KeyCode::PageDown,
    KeyCode::End,          KeyCode::Home,           KeyCode::Left,       KeyCode::Up,        KeyCode::Right,
    KeyCode::Down,         KeyCode::Select,         KeyCode::Print,      KeyCode::Undefined, KeyCode::PrintScreen,
    KeyCode::Insert,       KeyCode::Delete,         KeyCode::Undefined,  KeyCode::Num0,      KeyCode::Num1,
    KeyCode::Num2,         KeyCode::Num3,           KeyCode::Num4,       KeyCode::Num5,      KeyCode::Num6,
    KeyCode::Num7,         KeyCode::Num8,           KeyCode::Num9,       KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::A,            KeyCode::B,              KeyCode::C,          KeyCode::D,         KeyCode::E,
    KeyCode::F,            KeyCode::G,              KeyCode::H,          KeyCode::I,         KeyCode::J,
    KeyCode::K,            KeyCode::L,              KeyCode::M,          KeyCode::N,         KeyCode::O,
    KeyCode::P,            KeyCode::Q,              KeyCode::R,          KeyCode::S,         KeyCode::T,
    KeyCode::U,            KeyCode::V,              KeyCode::W,          KeyCode::X,         KeyCode::Y,
    KeyCode::Z,            KeyCode::LeftMenu,       KeyCode::RightMenu,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Numpad0,        KeyCode::Numpad1,    KeyCode::Numpad2,   KeyCode::Numpad3,
    KeyCode::Numpad4,      KeyCode::Numpad5,        KeyCode::Numpad6,    KeyCode::Numpad7,   KeyCode::Numpad8,
    KeyCode::Numpad9,      KeyCode::NumpadMultiply, KeyCode::NumpadAdd,  KeyCode::Undefined, KeyCode::NumpadSubtract,
    KeyCode::NumpadPeriod, KeyCode::NumpadDivide,   KeyCode::F1,         KeyCode::F2,        KeyCode::F3,
    KeyCode::F4,           KeyCode::F5,             KeyCode::F6,         KeyCode::F7,        KeyCode::F8,
    KeyCode::F9,           KeyCode::F10,            KeyCode::F11,        KeyCode::F12,       KeyCode::F13,
    KeyCode::F14,          KeyCode::F15,            KeyCode::F16,        KeyCode::F17,       KeyCode::F18,
    KeyCode::F19,          KeyCode::F20,            KeyCode::F21,        KeyCode::F22,       KeyCode::F23,
    KeyCode::F24,          KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::NumLock,
    KeyCode::ScrollLock,   KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::LeftShift,    KeyCode::RightShift,     KeyCode::LeftCtrl,   KeyCode::RightCtrl, KeyCode::LeftAlt,
    KeyCode::RightAlt,     KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Semicolon,      KeyCode::Equal,      KeyCode::Comma,     KeyCode::Minus,
    KeyCode::Period,       KeyCode::Slash,          KeyCode::BackQuote,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::LeftBracket,
    KeyCode::BackSlash,    KeyCode::RightBracket,   KeyCode::Quote,      KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,    KeyCode::Undefined,      KeyCode::Undefined,  KeyCode::Undefined, KeyCode::Undefined,
    KeyCode::Undefined,
};
// clang-format on

LRESULT CALLBACK YaGE::Window::__WindowProc__(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window *window = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, 0));
    if (window == nullptr) {
        switch (uMsg) {
        case WM_NCCREATE: {
            CREATESTRUCT *createStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
            SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
            break;
        }
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
    case WM_SETFOCUS: {
        // Handle focus callback.
        window->OnFocus(true);
        return 0;
    }

    case WM_KILLFOCUS: {
        window->OnFocus(false);
        return 0;
    }

    case WM_CLOSE: {
        window->OnClose();
        window->InternalDestroy();
        return 0;
    }

    case WM_CHAR:
    case WM_SYSCHAR: {
        if (wParam >= 0xD800 && wParam <= 0xDBFF) {
            // High surrogate.
            window->highSurrogate = static_cast<WCHAR>(wParam);
        } else {
            char32_t codePoint = 0;
            if (wParam >= 0xDC00 && wParam <= 0xDFFF) {
                if (window->highSurrogate) {
                    codePoint += (window->highSurrogate - 0xD800) << 10;
                    codePoint += static_cast<WCHAR>(wParam) - 0xDC00;
                    codePoint += 0x10000;
                }
            } else {
                codePoint = static_cast<WCHAR>(wParam);
            }

            window->highSurrogate = 0;

            // Handle char callback.
            ModifierKey mods = GetModifierKeyStatus();
            window->OnChar(codePoint, mods);
        }

        return 0;
    };

    case WM_UNICHAR: {
        if (wParam == UNICODE_NOCHAR)
            return TRUE;

        // Handle char callback.
        ModifierKey mods = GetModifierKeyStatus();
        window->OnChar(static_cast<char32_t>(wParam), mods);

        return 0;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP: {
        const KeyAction   action = (HIWORD(lParam) & KF_UP) ? KeyAction::Release : KeyAction::Press;
        const ModifierKey mods   = GetModifierKeyStatus();

        KeyCode key;

        UINT scanCode = (lParam & 0x00FF0000) >> 16;
        bool extended = (lParam & 0x01000000) != 0;

        switch (wParam) {
        case VK_SHIFT:
            key = KEY_CODE_MAP[MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX)];
            break;

        case VK_CONTROL:
            key = extended ? KeyCode::RightCtrl : KeyCode::LeftCtrl;
            break;

        case VK_MENU:
            key = extended ? KeyCode::RightAlt : KeyCode::LeftAlt;
            break;

        default:
            key = KEY_CODE_MAP[wParam];
            break;
        }

        window->OnKey(key, action, mods);
        break;
    }

    case WM_LBUTTONDOWN:
        window->OnKey(KeyCode::MouseLeft, KeyAction::Press, GetModifierKeyStatus());
        return 0;

    case WM_LBUTTONUP:
        window->OnKey(KeyCode::MouseLeft, KeyAction::Release, GetModifierKeyStatus());
        return 0;

    case WM_RBUTTONDOWN:
        window->OnKey(KeyCode::MouseRight, KeyAction::Press, GetModifierKeyStatus());
        return 0;

    case WM_RBUTTONUP:
        window->OnKey(KeyCode::MouseRight, KeyAction::Release, GetModifierKeyStatus());
        return 0;

    case WM_MBUTTONDOWN:
        window->OnKey(KeyCode::MouseMiddle, KeyAction::Press, GetModifierKeyStatus());
        return 0;

    case WM_MBUTTONUP:
        window->OnKey(KeyCode::MouseMiddle, KeyAction::Release, GetModifierKeyStatus());
        return 0;

    case WM_XBUTTONDOWN: {
        const KeyCode key = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? KeyCode::MouseX1 : KeyCode::MouseX2;
        window->OnKey(key, KeyAction::Press, GetModifierKeyStatus());
        return TRUE;
    }

    case WM_XBUTTONUP: {
        const KeyCode key = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? KeyCode::MouseX1 : KeyCode::MouseX2;
        window->OnKey(key, KeyAction::Release, GetModifierKeyStatus());
        return TRUE;
    }

    case WM_MOUSEMOVE: {
        const int32_t x = GET_X_LPARAM(lParam);
        const int32_t y = GET_Y_LPARAM(lParam);

        window->OnMouseMove(x, y);
        window->lastCursorPosX = x;
        window->lastCursorPosY = y;

        return 0;
    }

    case WM_MOUSEWHEEL: {
        const int32_t     delta = GET_WHEEL_DELTA_WPARAM(wParam);
        const int32_t     x     = GET_X_LPARAM(lParam);
        const int32_t     y     = GET_Y_LPARAM(lParam);
        const ModifierKey mods  = GetModifierKeyStatus();

        window->OnMouseWheel(x, y, 0, static_cast<float>(delta) / float(WHEEL_DELTA), mods);
        return 0;
    }

    case WM_MOUSEHWHEEL: {
        const int32_t     delta = GET_WHEEL_DELTA_WPARAM(wParam);
        const int32_t     x     = GET_X_LPARAM(lParam);
        const int32_t     y     = GET_Y_LPARAM(lParam);
        const ModifierKey mods  = GetModifierKeyStatus();

        window->OnMouseWheel(x, y, static_cast<float>(delta) / float(WHEEL_DELTA), 0, mods);
        return 0;
    }

    case WM_SIZE: {
        const uint32_t width  = LOWORD(lParam);
        const uint32_t height = HIWORD(lParam);

        if (wParam == SIZE_MINIMIZED)
            window->OnMinimized();
        else if (wParam == SIZE_MAXIMIZED)
            window->OnMaximized();

        window->OnResize(width, height);
        return 0;
    }

    case WM_MOVE: {
        const int32_t x = GET_X_LPARAM(lParam);
        const int32_t y = GET_Y_LPARAM(lParam);

        window->OnMove(x, y);
        return 0;
    }

    case WM_DROPFILES: {
        HDROP drop = reinterpret_cast<HDROP>(wParam);

        const uint32_t numFiles = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);

        std::vector<String> paths;
        paths.reserve(numFiles);

        // Query drag point.
        POINT point;
        DragQueryPoint(drop, &point);

        // Handle mouse move event.
        window->OnMouseMove(point.x, point.y);
        window->lastCursorPosX = point.x;
        window->lastCursorPosY = point.y;

        for (uint32_t i = 0; i < numFiles; ++i) {
            const uint32_t pathLength = DragQueryFileW(drop, i, nullptr, 0);

            String path;
            path.Resize(pathLength);
            DragQueryFileW(drop, i, reinterpret_cast<LPWSTR>(path.Data()), pathLength);

            paths.push_back(std::move(path));
        }

        window->OnFileDrop(point.x, point.y, numFiles, paths.data());
        DragFinish(drop);

        return 0;
    }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
