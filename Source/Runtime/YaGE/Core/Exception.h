#ifndef YAGE_CORE_EXCEPTION_H
#define YAGE_CORE_EXCEPTION_H

#include "String.h"

namespace YaGE {

/// @brief
///   Base class for all YaGE exceptions.
/// @note
///   This class inherits from std::exception so that you could catch all exceptions by simply catching @p std::exception, but you are not supposed to use @p std::exception::what() to get error message, since this library doesn't use @p char at all. Generally, I'd like to suggested that you catch @p YaGE::Exception for this library if necessary.
class Exception : public std::exception {
public:
    /// @brief
    ///   Create an empty exception.
    Exception() noexcept = default;

    /// @brief
    ///   Create an exception with a message.
    Exception(const char16_t *message) noexcept : message(message) {}

    /// @brief
    ///   Create an exception with a message.
    Exception(StringView message) noexcept : message(message) {}

    /// @brief
    ///   Create an exception with a message.
    Exception(String &&message) noexcept : message(std::move(message)) {}

    /// @brief
    ///   Destroy this exception object.
    YAGE_API ~Exception() noexcept override;

    /// @brief
    ///   Deprecated. This method doesn't return any useful message. Use @p Message() instead.
    [[deprecated]] auto what() const noexcept -> const char * override;

    /// @brief
    ///   Get the error message of this exception.
    ///
    /// @return StringView
    ///   The error message of this exception.
    YAGE_NODISCARD virtual auto Message() const noexcept -> StringView { return message; }

protected:
    /// @brief  Error message of this exception.
    String message;
};

class SystemErrorException : public Exception {
public:
    /// @brief
    ///   Create an empty system error exception.
    SystemErrorException() noexcept = default;

    /// @brief
    ///   Create an exception with a message.
    SystemErrorException(const char16_t *message) noexcept : Exception(message) {}

    /// @brief
    ///   Create an exception with a message.
    SystemErrorException(StringView message) noexcept : Exception(message) {}

    /// @brief
    ///   Create an exception with a message.
    SystemErrorException(String &&message) noexcept : Exception(std::move(message)) {}

    /// @brief
    ///   Create an exception with a system error code.
    YAGE_API SystemErrorException(int32_t errorCode) noexcept;

    /// @brief
    ///   Create an exception with a system error code and a message.
    ///
    /// @param errorCode    System error code.
    /// @param message      Error message.
    SystemErrorException(int32_t errorCode, const char16_t *message) noexcept
        : Exception(message), errorCode(errorCode) {}

    /// @brief
    ///   Create an exception with a system error code and a message.
    ///
    /// @param errorCode    System error code.
    /// @param message      Error message.
    SystemErrorException(int32_t errorCode, StringView message) noexcept : Exception(message), errorCode(errorCode) {}

    /// @brief
    ///   Create an exception with a system error code and a message.
    ///
    /// @param errorCode    System error code.
    /// @param message      Error message.
    SystemErrorException(int32_t errorCode, String &&message) noexcept
        : Exception(std::move(message)), errorCode(errorCode) {}

    /// @brief
    ///   Destroy this exception object.
    YAGE_API ~SystemErrorException() noexcept override;

    /// @brief
    ///   Get system error code of this exception.
    ///
    /// @return int32_t
    ///   System error code of this exception.
    YAGE_NODISCARD auto ErrorCode() const noexcept -> int32_t { return errorCode; }

protected:
    /// @brief  System error code of this exception.
    int32_t errorCode = 0;
};

class RenderAPIException : public SystemErrorException {
public:
    using SystemErrorException::SystemErrorException;

    /// @brief
    ///   Destroy this exception object.
    YAGE_API ~RenderAPIException() noexcept override;
};

} // namespace YaGE

#endif // YAGE_CORE_EXCEPTION_H
