#ifndef YAGE_CORE_WINDOW_H
#define YAGE_CORE_WINDOW_H

#include "String.h"

#include <Windows.h>

namespace YaGE {

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

enum class CursorMode {
    Normal,
    Hidden,
    Disabled,
};

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

    friend class SwapChain;

private:
    /// @brief
    ///   For internal usage. Win32 window procedure.
    static LRESULT CALLBACK __WindowProc__(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    /// @brief
    ///   For internal usage. Disable cursor.
    auto DisableCursor() noexcept -> void;

    /// @brief
    ///   For internal usage. Enable cursor.
    auto EnableCursor() noexcept -> void;

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

    /// @brief  Cursor X position before entering disabled mode.
    uint32_t restoreCursorPosX;

    /// @brief  Cursor Y position before entering disabled mode.
    uint32_t restoreCursorPosY;

    /// @brief  Last cursor X position.
    uint32_t lastCursorPosX;

    /// @brief  Last cursor Y position.
    uint32_t lastCursorPosY;

    /// @brief  Last high surrogate of a UTF-16 character.
    WCHAR highSurrogate;

    /// @brief  Specifies whether client area is active.
    bool clientIsActive;

    /// @brief  Cursor mode of this window.
    CursorMode cursorMode;
};

} // namespace YaGE

#endif // YAGE_CORE_WINDOW_H
