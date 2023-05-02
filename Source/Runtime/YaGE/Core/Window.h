#pragma once

#include "String.h"

#include <Windows.h>

namespace YaGE {

enum class KeyCode {
    Undefined      = -1,
    MouseLeft      = VK_LBUTTON,
    MouseRight     = VK_RBUTTON,
    Break          = VK_CANCEL,
    MouseMiddle    = VK_MBUTTON,
    MouseX1        = VK_XBUTTON1,
    MouseX2        = VK_XBUTTON2,
    Backspace      = VK_BACK,
    Tab            = VK_TAB,
    Clear          = VK_CLEAR,
    Enter          = VK_RETURN,
    Pause          = VK_PAUSE,
    CapsLock       = VK_CAPITAL,
    Escape         = VK_ESCAPE,
    Space          = VK_SPACE,
    PageUp         = VK_PRIOR,
    PageDown       = VK_NEXT,
    End            = VK_END,
    Home           = VK_HOME,
    Left           = VK_LEFT,
    Up             = VK_UP,
    Right          = VK_RIGHT,
    Down           = VK_DOWN,
    Select         = VK_SELECT,
    Print          = VK_PRINT,
    PrintScreen    = VK_SNAPSHOT,
    Insert         = VK_INSERT,
    Delete         = VK_DELETE,
    Num0           = 0x30,
    Num1           = 0x31,
    Num2           = 0x32,
    Num3           = 0x33,
    Num4           = 0x34,
    Num5           = 0x35,
    Num6           = 0x36,
    Num7           = 0x37,
    Num8           = 0x38,
    Num9           = 0x39,
    A              = 0x41,
    B              = 0x42,
    C              = 0x43,
    D              = 0x44,
    E              = 0x45,
    F              = 0x46,
    G              = 0x47,
    H              = 0x48,
    I              = 0x49,
    J              = 0x4A,
    K              = 0x4B,
    L              = 0x4C,
    M              = 0x4D,
    N              = 0x4E,
    O              = 0x4F,
    P              = 0x50,
    Q              = 0x51,
    R              = 0x52,
    S              = 0x53,
    T              = 0x54,
    U              = 0x55,
    V              = 0x56,
    W              = 0x57,
    X              = 0x58,
    Y              = 0x59,
    Z              = 0x5A,
    LeftMenu       = VK_LWIN,
    RightMenu      = VK_RWIN,
    Numpad0        = VK_NUMPAD0,
    Numpad1        = VK_NUMPAD1,
    Numpad2        = VK_NUMPAD2,
    Numpad3        = VK_NUMPAD3,
    Numpad4        = VK_NUMPAD4,
    Numpad5        = VK_NUMPAD5,
    Numpad6        = VK_NUMPAD6,
    Numpad7        = VK_NUMPAD7,
    Numpad8        = VK_NUMPAD8,
    Numpad9        = VK_NUMPAD9,
    NumpadMultiply = VK_MULTIPLY,
    NumpadAdd      = VK_ADD,
    NumpadSubtract = VK_SUBTRACT,
    NumpadPeriod   = VK_DECIMAL,
    NumpadDivide   = VK_DIVIDE,
    F1             = VK_F1,
    F2             = VK_F2,
    F3             = VK_F3,
    F4             = VK_F4,
    F5             = VK_F5,
    F6             = VK_F6,
    F7             = VK_F7,
    F8             = VK_F8,
    F9             = VK_F9,
    F10            = VK_F10,
    F11            = VK_F11,
    F12            = VK_F12,
    F13            = VK_F13,
    F14            = VK_F14,
    F15            = VK_F15,
    F16            = VK_F16,
    F17            = VK_F17,
    F18            = VK_F18,
    F19            = VK_F19,
    F20            = VK_F20,
    F21            = VK_F21,
    F22            = VK_F22,
    F23            = VK_F23,
    F24            = VK_F24,
    NumLock        = VK_NUMLOCK,
    ScrollLock     = VK_SCROLL,
    LeftShift      = VK_LSHIFT,
    RightShift     = VK_RSHIFT,
    LeftCtrl       = VK_LCONTROL,
    RightCtrl      = VK_RCONTROL,
    LeftAlt        = VK_LMENU,
    RightAlt       = VK_RMENU,
    Semicolon      = VK_OEM_1,
    Equal          = VK_OEM_PLUS,
    Comma          = VK_OEM_COMMA,
    Minus          = VK_OEM_MINUS,
    Period         = VK_OEM_PERIOD,
    Slash          = VK_OEM_2,
    BackQuote      = VK_OEM_3,
    LeftBracket    = VK_OEM_4,
    BackSlash      = VK_OEM_5,
    RightBracket   = VK_OEM_6,
    Quote          = VK_OEM_7,
};

enum class KeyAction {
    Press,
    Release,
    Repeat,
};

enum class ModifierKey {
    None    = 0,
    Shift   = (1 << 0),
    Control = (1 << 1),
    Alt     = (1 << 2),
    Super   = (1 << 3),
};

constexpr auto operator|(ModifierKey a, ModifierKey b) -> ModifierKey {
    return static_cast<ModifierKey>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr auto operator&(ModifierKey a, ModifierKey b) -> ModifierKey {
    return static_cast<ModifierKey>(static_cast<int>(a) & static_cast<int>(b));
}

constexpr auto operator^(ModifierKey a, ModifierKey b) -> ModifierKey {
    return static_cast<ModifierKey>(static_cast<int>(a) ^ static_cast<int>(b));
}

constexpr auto operator~(ModifierKey a) -> ModifierKey { return static_cast<ModifierKey>(~static_cast<int>(a)); }

constexpr auto operator|=(ModifierKey &a, ModifierKey b) -> ModifierKey & { return a = a | b; }

constexpr auto operator&=(ModifierKey &a, ModifierKey b) -> ModifierKey & { return a = a & b; }

constexpr auto operator^=(ModifierKey &a, ModifierKey b) -> ModifierKey & { return a = a ^ b; }

enum class WindowStyle {
    None        = 0,
    Titled      = (1 << 0),
    BorderLess  = (1 << 1),
    Minimizable = (1 << 2),
    Maximizable = (1 << 3),
    Resizable   = (1 << 4),
    TopMost     = (1 << 5),
    FileDrop    = (1 << 6),

    Default    = Titled,
    Overlapped = (Default | Minimizable | Maximizable | Resizable),
};

constexpr auto operator|(WindowStyle a, WindowStyle b) -> WindowStyle {
    return static_cast<WindowStyle>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr auto operator&(WindowStyle a, WindowStyle b) -> WindowStyle {
    return static_cast<WindowStyle>(static_cast<int>(a) & static_cast<int>(b));
}

constexpr auto operator^(WindowStyle a, WindowStyle b) -> WindowStyle {
    return static_cast<WindowStyle>(static_cast<int>(a) ^ static_cast<int>(b));
}

constexpr auto operator~(WindowStyle a) -> WindowStyle { return static_cast<WindowStyle>(~static_cast<int>(a)); }

constexpr auto operator|=(WindowStyle &a, WindowStyle b) -> WindowStyle & { return a = a | b; }

constexpr auto operator&=(WindowStyle &a, WindowStyle b) -> WindowStyle & { return a = a & b; }

constexpr auto operator^=(WindowStyle &a, WindowStyle b) -> WindowStyle & { return a = a ^ b; }

class Window {
public:
    /// @brief
    ///     Create a new window.
    ///
    /// @param title    Title of this window.
    /// @param width    Client width of this window.
    /// @param height   Client height of this window.
    /// @param style    Style of this window.
    ///
    /// @throw SystemErrorException
    ///   Thrown if failed to create Win32 objects.
    YAGE_API Window(StringView title, uint32_t width, uint32_t height, WindowStyle style = WindowStyle::Default);

    /// @brief
    ///   Copy constructor is disabled.
    Window(const Window &) = delete;

    /// @brief
    ///   Copy assignment operator is disabled.
    auto operator=(const Window &) = delete;

    /// @brief
    ///   Destroy this window.
    YAGE_API virtual ~Window() noexcept;

    /// @brief
    ///   Get the title of this window.
    ///
    /// @return StringView
    ///   Return title of this window.
    YAGE_NODISCARD auto Title() const noexcept -> StringView { return title; }

    /// @brief
    ///   Set a new title of this window.
    ///
    /// @param newTitle New title of this window.
    YAGE_API auto SetTitle(StringView newTitle) noexcept -> void;

    /// @brief
    ///   Checks if this window has been closed.
    ///
    /// @return bool
    /// @retval true   This window has been closed.
    /// @retval false  This window is still open.
    YAGE_NODISCARD auto IsClosed() const noexcept -> bool { return hWnd == nullptr; }

    /// @brief
    ///   Get client width of this window.
    ///
    /// @return uint32_t
    ///   Return client width of this window.
    YAGE_NODISCARD auto ClientWidth() const noexcept -> uint32_t { return clientWidth; }

    /// @brief
    ///   Get client height of this window.
    ///
    /// @return uint32_t
    ///   Return client height of this window.
    YAGE_NODISCARD auto ClientHeight() const noexcept -> uint32_t { return clientHeight; }

    /// @brief
    ///   Resize client area of this window.
    ///
    /// @param width    New client width.
    /// @param height   New client height.
    YAGE_API auto Resize(uint32_t width, uint32_t height) noexcept -> void;

    /// @brief
    ///   Move this window to the center of screen.
    YAGE_API auto Center() noexcept -> void;

    /// @brief
    ///   Show this this window if hidden.
    auto Show() noexcept -> void { ShowWindow(hWnd, SW_SHOW); }

    /// @brief
    ///   Hide this window if shown.
    auto Hide() noexcept -> void { ShowWindow(hWnd, SW_HIDE); }

protected:
    /// @brief
    ///   Focus change callback for this window.
    ///
    /// @param focused  Specifies whether this window is focused.
    YAGE_API virtual auto OnFocus(bool focused) -> void;

    /// @brief
    ///   Close callback for this window.
    YAGE_API virtual auto OnClose() -> void;

    /// @brief
    ///   Text input callback for this window.
    ///
    /// @param codePoint    Unicode character code point.
    /// @param mods         Current modifier keys.
    YAGE_API virtual auto OnChar(char32_t codePoint, ModifierKey mods) -> void;

    /// @brief
    ///   Key input callback for this window.
    ///
    /// @param key    Input key code.
    /// @param action Input key action.
    /// @param mods   Current modifier keys.
    YAGE_API virtual auto OnKey(KeyCode key, KeyAction action, ModifierKey mods) -> void;

    /// @brief
    ///   Mouse move callback for this window.
    ///
    /// @param x Mouse X position. Screen coordinate starts from left to right.
    /// @param y Mouse Y position. Screen coordinate starts from top to bottom.
    YAGE_API virtual auto OnMouseMove(int32_t x, int32_t y) -> void;

    /// @brief
    ///   Mouse wheel callback for this window.
    ///
    /// @param x       Mouse X position.
    /// @param y       Mouse Y position.
    /// @param deltaX  Mouse wheel X offset.
    /// @param deltaY  Mouse wheel Y offset.
    /// @param mods    Current modifier keys.
    YAGE_API virtual auto OnMouseWheel(int32_t x, int32_t y, float deltaX, float deltaY, ModifierKey mods) -> void;

    /// @brief
    ///   Window minimized callback.
    YAGE_API virtual auto OnMinimized() -> void;

    /// @brief
    ///   Window maximized callback.
    YAGE_API virtual auto OnMaximized() -> void;

    /// @brief
    ///   Window resize callback.
    ///
    /// @param width    New client width.
    /// @param height   New client height.
    YAGE_API virtual auto OnResize(uint32_t width, uint32_t height) -> void;

    /// @brief
    ///   Window move callback.
    ///
    /// @param x New window X position.
    /// @param y New window Y position.
    YAGE_API virtual auto OnMove(int32_t x, int32_t y) -> void;

    /// @brief
    ///   Window file drop callback.
    ///
    /// @param x      Mouse X position.
    /// @param y      Mouse Y position.
    /// @param count  Number of files dropped.
    /// @param paths  An array of dropped file path.
    YAGE_API virtual auto OnFileDrop(int32_t x, int32_t y, uint32_t count, String paths[]) -> void;

    friend class SwapChain;

private:
    /// @brief
    ///   For internal usage. Win32 window procedure.
    static LRESULT CALLBACK __WindowProc__(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    /// @brief
    ///   For internal usage. Destroy this window.
    auto InternalDestroy() noexcept -> void;

protected:
    /// @brief  Win32 HINSTANCE object.
    HINSTANCE hInstance;

    /// @brief  Win32 HWND object.
    HWND hWnd;

private:
    /// @brief  Title of this window.
    String title;

    /// @brief  Client width of this window.
    uint32_t clientWidth;

    /// @brief  Client height of this window.
    uint32_t clientHeight;

    /// @brief  Last cursor X position.
    int32_t lastCursorPosX;

    /// @brief  Last cursor Y position.
    int32_t lastCursorPosY;

    /// @brief  Last high surrogate of a UTF-16 character.
    WCHAR highSurrogate;
};

} // namespace YaGE
