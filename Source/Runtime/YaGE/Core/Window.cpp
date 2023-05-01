#include "Window.h"
#include "Exception.h"

#include <shellscalingapi.h>

using namespace YaGE;

namespace {

static LRESULT CALLBACK WindowInitializeProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept {
    if (uMsg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCT *>(lParam);
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static auto SupportWindows10BuildFeatures(WORD build) noexcept {
    OSVERSIONINFOEX info{
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

    return VerifyVersionInfo(&info, mask, condition) != FALSE;
}

class WindowClass {
public:
    WindowClass() : hInstance(GetModuleHandle(nullptr)) {
        const WNDCLASSEX clsEx{
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

        classId = RegisterClassEx(&clsEx);
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
    ~WindowClass() noexcept { UnregisterClass(reinterpret_cast<LPCTSTR>(classId), hInstance); }

    /// @brief
    ///   Allow implicit conversion to LPCWSTR.
    operator LPCTSTR() const noexcept { return reinterpret_cast<LPCTSTR>(classId); }

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
    : hInstance(GetModuleHandle(nullptr)),
      hWnd(nullptr),
      title(title),
      clientWidth(),
      clientHeight(),
      restoreCursorPosX(),
      restoreCursorPosY(),
      lastCursorPosX(),
      lastCursorPosY(),
      highSurrogate(),
      clientIsActive(),
      cursorMode(CursorMode::Normal) {
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
    hWnd = CreateWindowEx(dwStyleEx, cls, reinterpret_cast<LPCWSTR>(this->title.Data()), dwStyle, CW_USEDEFAULT,
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
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Window::__WindowProc__));
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

auto YaGE::Window::DisableCursor() noexcept -> void {
    { // Store current cursor position.
        POINT pos;
        GetCursorPos(&pos);
        ScreenToClient(hWnd, &pos);

        this->restoreCursorPosX = pos.x;
        this->restoreCursorPosY = pos.y;
    }

    // Hide cursor.
    SetCursor(nullptr);

    // Center cursor.
    RECT rect;
    GetClientRect(hWnd, &rect);

    uint32_t width  = static_cast<uint32_t>(rect.right - rect.left);
    uint32_t height = static_cast<uint32_t>(rect.bottom - rect.top);
    POINT    newPos{static_cast<LONG>(width / 2), static_cast<LONG>(height / 2)};

    this->lastCursorPosX = static_cast<uint32_t>(newPos.x);
    this->lastCursorPosY = static_cast<uint32_t>(newPos.y);

    ClientToScreen(hWnd, &newPos);
    SetCursorPos(newPos.x, newPos.y);

    // Update cursor clip rect.
    ClientToScreen(hWnd, reinterpret_cast<POINT *>(&rect.left));
    ClientToScreen(hWnd, reinterpret_cast<POINT *>(&rect.right));
    ClipCursor(&rect);
}

auto YaGE::Window::EnableCursor() noexcept -> void {
    ClipCursor(nullptr);

    POINT restorePos{static_cast<LONG>(this->restoreCursorPosX), static_cast<LONG>(this->restoreCursorPosY)};
    ClientToScreen(hWnd, &restorePos);
    SetCursorPos(restorePos.x, restorePos.y);

    SetCursor(LoadCursor(nullptr, IDC_ARROW));
}

auto YaGE::Window::InternalDestroy() noexcept -> void {
    if (hWnd == nullptr)
        return;

    DestroyWindow(hWnd);
    hWnd = nullptr;
}

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
    case WM_MOUSEACTIVATE: {
        if (HIWORD(lParam) == WM_LBUTTONDOWN) {
            if (LOWORD(lParam) == HTCLIENT)
                window->clientIsActive = true;
        }

        break;
    }

    case WM_CAPTURECHANGED: {
        if (lParam == 0 && window->clientIsActive) {
            if (window->cursorMode == CursorMode::Disabled)
                window->DisableCursor();

            window->clientIsActive = false;
        }

        break;
    }

    case WM_SETFOCUS: {
        // Handle focus callback.
        window->OnFocus(true);

        // Do not disable cursor when the user is interacting with a caption button.
        if (window->clientIsActive)
            break;

        if (window->cursorMode == CursorMode::Disabled)
            window->DisableCursor();

        return 0;
    }

    case WM_KILLFOCUS: {
        if (window->cursorMode == CursorMode::Disabled)
            window->EnableCursor();

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
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
