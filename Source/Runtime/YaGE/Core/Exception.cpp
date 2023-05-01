#include "Exception.h"

#include <Windows.h>

using namespace YaGE;

YaGE::Exception::~Exception() noexcept {}

auto YaGE::Exception::what() const noexcept -> const char * { return "YaGE::Exception::what() is deprecated."; }

YAGE_NODISCARD static auto SystemErrorMessage(int32_t errorCode) noexcept -> String {
    char16_t buffer[2048];
    DWORD    charCount;

    charCount = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), reinterpret_cast<LPWSTR>(buffer),
                              static_cast<DWORD>(std::size(buffer)), nullptr);
    if (charCount <= 0)
        return String(u"Unknown system error.");

    StringView msg(buffer, static_cast<size_t>(charCount));
    msg.Trim();

    return String(msg);
}

YaGE::SystemErrorException::SystemErrorException(int32_t errorCode) noexcept
    : Exception(SystemErrorMessage(errorCode)), errorCode(errorCode) {}

YaGE::SystemErrorException::~SystemErrorException() noexcept {}

YaGE::RenderAPIException::~RenderAPIException() noexcept {}
