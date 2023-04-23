#ifndef YAGE_CORE_STRING_VIEW_H
#define YAGE_CORE_STRING_VIEW_H

#include "Hash.h"

#include <fmt/format.h>

#include <vector>

namespace YaGE {

class StringView {
public:
    using traits_type     = std::char_traits<char16_t>;
    using value_type      = char16_t;
    using pointer         = value_type *;
    using const_pointer   = const value_type *;
    using reference       = value_type &;
    using const_reference = const value_type &;
    using const_iterator  = const_pointer;
    using iterator        = const_iterator;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    /// @brief
    ///   Create an empty StringView object.
    constexpr StringView() noexcept = default;

#if YAGE_CPLUSPLUS >= 201703L
    /// @brief
    ///   Create a StringView object from std::basic_string_view.
    ///
    /// @param str    The std::basic_string_view.
    constexpr StringView(std::basic_string_view<value_type> str) noexcept : ptr(str.data()), len(str.size()) {}
#endif

    /// @brief
    ///   Create a StringView object from a sequence of char16_t characters.
    ///
    /// @param str    Pointer to start of the char16_t sequence.
    /// @param count  Number of char16_t characters in the sequence.
    constexpr StringView(const_pointer str, size_type count) noexcept : ptr(str), len(count) {}

    /// @brief
    ///   Create a StringView object from a null-terminated UTF-16 string.
    ///
    /// @param str  Pointer to start of the null-terminated UTF-16 string.
    YAGE_CONSTEXPR17 StringView(const_pointer str) noexcept : ptr(str), len(traits_type::length(str)) {}

    /// @brief
    ///   Creating StringView from nullptr is disabled.
    StringView(std::nullptr_t) = delete;

    /// @brief
    ///   Get iterator to the first character in this StringView.
    ///
    /// @return const_iterator
    ///   Return iterator to the first character in this StringView.
    YAGE_NODISCARD constexpr auto begin() const noexcept -> const_iterator { return ptr; }

    /// @brief
    ///   Get iterator to the position after the last character in this StringView.
    ///
    /// @return const_iterator
    ///   Return iterator to the position after the last character in this StringView.
    YAGE_NODISCARD constexpr auto end() const noexcept -> const_iterator { return ptr + len; }

    /// @brief
    ///   Access the @p index'th character in this StringView.
    /// @note
    ///   No index checking is performed.
    ///
    /// @param index  Index of the character to access.
    ///
    /// @return const_reference
    ///   Return the @p index'th character in this StringView.
    constexpr auto operator[](size_type index) const noexcept -> const_reference { return ptr[index]; }

    /// @brief
    ///   Access the @p index'th character in this StringView.
    /// @note
    ///   This method is exactly the same as operator[]. No index checking is performed.
    ///
    /// @param index  Index of the character to access.
    ///
    /// @return const_reference
    ///   Return the @p index'th character in this StringView.
    YAGE_NODISCARD constexpr auto At(size_type index) const noexcept -> const_reference { return ptr[index]; }

    /// @brief
    ///   Get the first character in this StringView.
    /// @note
    ///   No index checking is performed.
    ///
    /// @return const_reference
    ///   Return reference to the first character in this StringView.
    YAGE_NODISCARD constexpr auto Front() const noexcept -> const_reference { return ptr[0]; }

    /// @brief
    ///   Get the last character in this StringView.
    /// @note
    ///   No index checking is performed.
    ///
    /// @return const_reference
    ///   Return reference to the last character in this StringView.
    YAGE_NODISCARD constexpr auto Back() const noexcept -> const_reference { return ptr[len - 1]; }

    /// @brief
    ///   Get pointer to start of this StringView.
    ///
    /// @return const_pointer
    YAGE_NODISCARD constexpr auto Data() const noexcept -> const_pointer { return ptr; }

    /// @brief
    ///   Get number of characters in this StringView.
    ///
    /// @return size_type
    ///   Return number of characters in this StringView.
    YAGE_NODISCARD constexpr auto Length() const noexcept -> size_type { return len; }

    /// @brief
    ///   Checks if this StringView is empty.
    ///
    /// @return bool
    /// @retval true   This StringView is empty.
    /// @retval false  This StringView is not empty.
    YAGE_NODISCARD constexpr auto IsEmpty() const noexcept -> bool { return len == 0; }

    /// @brief
    ///   Checks if this StringView is null-terminated.
    /// @note
    ///   It is safe to call this method if pointer is @p nullptr, but this method may still cause segmentation fault in certain conditions.
    ///
    /// @return bool
    /// @retval true   This StringView is null-terminated.
    /// @retval false  This StringView is not null-terminated.
    YAGE_NODISCARD constexpr auto IsNullTerminated() const noexcept -> bool { return ptr && ptr[len] == value_type(); }

    /// @brief
    ///   Remove the first @p count characters from this StringView.
    ///
    /// @param count  Number of characters to remove. All characters will be removed if @p count is greater than length of this StringView.
    ///
    /// @return StringView &
    ///   Return reference to this StringView.
    constexpr auto RemovePrefix(size_type count) noexcept -> StringView & {
        count = (count > len ? len : count);
        ptr += count;
        len -= count;
        return *this;
    }

    /// @brief
    ///   Remove the last @p count characters from this StringView.
    ///
    /// @param count  Number of characters to remove. All characters will be removed if @p count is greater than length of this StringView.
    ///
    /// @return StringView &
    ///   Return reference to this StringView.
    constexpr auto RemoveSuffix(size_type count) noexcept -> StringView & {
        count = (count > len ? len : count);
        len -= count;
        return *this;
    }

    /// @brief
    ///   Copy part of this StringView to the specified memory.
    ///
    /// @param[out] dest      Pointer to the copy desetination.
    /// @param      count     Expected number of characters to be copied. This value may be clamped if greater than length of this StringView.
    /// @param      from      Start index of this StringView to be copied.
    ///
    /// @return size_type
    ///   Return actual number of characters copied.
    YAGE_CONSTEXPR20 auto CopyTo(pointer dest, size_type count, size_type from = 0) const noexcept -> size_type {
        if (from >= len)
            return 0;

        const const_pointer copyStart = ptr + from;
        const size_type     maxCount  = len - from;

        count = (count > maxCount ? maxCount : count);
        traits_type::copy(dest, copyStart, count);

        return count;
    }

    /// @brief
    ///   Create a sub-string from the @p from'th character to the end of this StringView.
    ///
    /// @param from   Index of the first character in the sub-string.
    ///
    /// @return StringView
    ///   Return a new StringView that contains the sub-string. An empty StringView will be returned if @p from is greater than length of this StringView.
    YAGE_NODISCARD constexpr auto SubString(size_type from) const noexcept -> StringView {
        if (from > len)
            return {};
        return {ptr + from, len - from};
    }

    /// @brief
    ///   Create a sub-string from the @p from'th character with @p count character.
    ///
    /// @param from   Index of the first character in the sub-string.
    /// @param count  Number of characters in the sub-string.
    ///
    /// @return StringView
    ///   Return a new StringView that contains the specified sub-string.
    YAGE_NODISCARD constexpr auto SubString(size_type from, size_type count) const noexcept -> StringView {
        if (from > len)
            return {};

        const const_pointer strStart = ptr + from;
        const size_type     maxCount = len - from;

        count = (count > maxCount ? maxCount : count);
        return {strStart, count};
    }

    /// @brief
    ///   Compare two StringView lexically.
    ///
    /// @param rhs  The StringView to compare with.
    ///
    /// @return int
    /// @retval -1  This StringView is lexically less than @p rhs.
    /// @retval 0   This StringView is lexically equal to @p rhs.
    /// @retval 1   This StringView is lexically greater than @p rhs.
    YAGE_NODISCARD YAGE_CONSTEXPR17 auto Compare(StringView rhs) const noexcept -> int {
        const size_type cmpLen = (len < rhs.len ? len : rhs.len);

        int result = traits_type::compare(ptr, rhs.ptr, cmpLen);
        if (result == 0)
            result = (len < rhs.len ? -1 : (len > rhs.len ? 1 : 0));

        return result;
    }

    /// @brief
    ///   Checks if this StringView starts with the specified string.
    ///
    /// @param str  The StringView to be compared with start part of this StringView.
    ///
    /// @return bool
    /// @retval true   This StringView starts with @p str.
    /// @retval false  This StringView does not start with @p str.
    YAGE_NODISCARD constexpr auto StartsWith(StringView str) const noexcept -> bool {
        if (str.len > len)
            return false;
        return traits_type::compare(ptr, str.ptr, str.len) == 0;
    }

    /// @brief
    ///   Checks if this StringView starts with the specified character.
    ///
    /// @param ch       The character to be compared with the first character in this StringView.
    ///
    /// @return bool
    /// @retval true    This StringView starts with character @p ch.
    /// @retval false   This StringView doesn't start with character @p ch.
    YAGE_NODISCARD constexpr auto StartsWith(value_type ch) const noexcept -> bool {
        if (len == 0)
            return false;
        return *ptr == ch;
    }

    /// @brief
    ///   Checks if this StringView ends with the specified string.
    ///
    /// @param str  The StringView to be compared with end part of this StringView.
    ///
    /// @return bool
    /// @retval true   This StringView ends with @p str.
    /// @retval false  This StringView does not end with @p str.
    YAGE_NODISCARD constexpr auto EndsWith(StringView str) const noexcept -> bool {
        if (str.len > len)
            return false;
        return traits_type::compare(ptr + len - str.len, str.ptr, str.len) == 0;
    }

    /// @brief
    ///   Checks if this StringView ends with the specified character.
    ///
    /// @param ch       The character to be compared with the last character in this StringView.
    ///
    /// @return bool
    /// @retval true    This StringView ends with character @p ch.
    /// @retval false   This StringView doesn't end with character @p ch.
    YAGE_NODISCARD constexpr auto EndsWith(value_type ch) const noexcept -> bool {
        if (len == 0)
            return false;
        return ptr[len - 1] == ch;
    }

    /// @brief
    ///   Checks if this StringView contains the specified string.
    ///
    /// @param str  The StringView to be searched in this StringView.
    ///
    /// @return bool
    /// @retval true   This StringView contains @p str.
    /// @retval false  This StringView does not contain @p str.
    YAGE_NODISCARD constexpr auto Contains(StringView str) const noexcept -> bool {
        if (str.len > len)
            return false;

        const const_pointer cmpEnd = ptr + len - str.len + 1;
        for (const_pointer i = ptr; i != cmpEnd; ++i) {
            if (traits_type::compare(i, str.ptr, str.len) == 0)
                return true;
        }
        return false;
    }

    /// @brief
    ///   Checks if this StringView contains the specified character.
    ///
    /// @param ch       The character to be searched in this StringView.
    ///
    /// @return bool
    /// @retval true    This StringView contains character @p ch.
    /// @retval false   This StringView doesn't contain character @p ch.
    YAGE_NODISCARD constexpr auto Contains(value_type ch) const noexcept -> bool {
        const const_pointer cmpEnd = ptr + len;
        for (const_pointer i = ptr; i != cmpEnd; ++i) {
            if (traits_type::eq(*i, ch))
                return true;
        }
        return false;
    }

    /// @brief
    ///   Find the first occurrence of the specified string in this StringView.
    ///
    /// @param str  The StringView to be searched in this StringView.
    /// @param from The index to start searching from.
    ///
    /// @return size_type
    ///   Return the index of the first occurrence of the specified string if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto IndexOf(StringView str, size_type from = 0) const noexcept -> size_type {
        if (str.len + from > len)
            return size_type(-1);

        const const_pointer cmpEnd = ptr + len - str.len + 1;
        for (const_pointer i = ptr + from; i != cmpEnd; ++i) {
            if (traits_type::compare(i, str.ptr, str.len) == 0)
                return static_cast<size_type>(i - ptr);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the first occurrence of the specified character in this StringView.
    ///
    /// @param ch   The character to be searched in this StringView.
    /// @param from The index to start searching from.
    ///
    /// @return size_type
    ///   Return the index of the first occurrence of the specified character if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto IndexOf(value_type ch, size_type from = 0) const noexcept -> size_type {
        if (from >= len)
            return size_type(-1);

        const const_pointer cmpEnd = ptr + len;
        for (const_pointer i = ptr; i != cmpEnd; ++i) {
            if (traits_type::eq(*i, ch))
                return static_cast<size_type>(i - ptr);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the last occurrence of the specified string in this StringView.
    ///
    /// @param str  The StringView to be searched in this StringView.
    ///
    /// @return size_type
    ///   Return the index of the last occurrence of the specified string if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto LastIndexOf(StringView str) const noexcept -> size_type {
        if (str.len > len)
            return size_type(-1);

        const const_pointer searchStart = ptr + len - str.len + 1;
        for (const_pointer i = searchStart; i != ptr; --i) {
            if (traits_type::compare(i - 1, str.ptr, str.len) == 0)
                return static_cast<size_type>(i - ptr - 1);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the last occurrence of the specified string in this StringView.
    ///
    /// @param str  The StringView to be searched in this StringView.
    /// @param from The index to start searching from.
    ///
    /// @return size_type
    ///   Return the index of the last occurrence of the specified string if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto LastIndexOf(StringView str, size_type from) const noexcept -> size_type {
        if (str.len > len)
            return size_type(-1);

        const const_pointer searchStart = (from + str.len > len ? ptr + len - str.len + 1 : ptr + from + 1);

        for (const_pointer i = searchStart; i != ptr; --i) {
            if (traits_type::compare(i - 1, str.ptr, str.len) == 0)
                return static_cast<size_type>(i - ptr - 1);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the last occurrence of the specified character in this StringView.
    ///
    /// @param ch   The character to be searched in this StringView.
    ///
    /// @return size_type
    ///   Return the index of the last occurrence of the specified character if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto LastIndexOf(value_type ch) const noexcept -> size_type {
        if (len == 0)
            return size_type(-1);

        for (const_pointer i = ptr + len; i != ptr; --i) {
            if (traits_type::eq(*(i - 1), ch))
                return static_cast<size_type>(i - ptr - 1);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the last occurrence of the specified character in this StringView.
    ///
    /// @param ch   The character to be searched in this StringView.
    /// @param from The index to start searching from.
    ///
    /// @return size_type
    ///   Return the index of the last occurrence of the specified character if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto LastIndexOf(value_type ch, size_type from) const noexcept -> size_type {
        if (len == 0)
            return size_type(-1);

        const const_pointer searchStart = (from + 1 > len ? ptr + len : ptr + from + 1);

        for (const_pointer i = searchStart; i != ptr; --i) {
            if (traits_type::eq(*(i - 1), ch))
                return static_cast<size_type>(i - ptr - 1);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the first occurrence of any character in the specified string in this StringView.
    ///
    /// @param charSet  The string containing the characters to be searched in this StringView.
    /// @param from     The index to start searching from.
    ///
    /// @return size_type
    ///   Return the index of the first occurrence of any character in the specified string if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto IndexOfAny(StringView charSet, size_type from = 0) const noexcept -> size_type {
        if (from > len)
            return size_type(-1);

        const const_pointer strEnd = ptr + len;
        for (const_pointer i = ptr + from; i != strEnd; ++i) {
            if (charSet.Contains(*i))
                return static_cast<size_type>(i - ptr);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the last occurrence of any character in the specified string in this StringView.
    ///
    /// @param charSet  The string containing the characters to be searched in this StringView.
    ///
    /// @return size_type
    ///   Return the index of the last occurrence of any character in the specified string if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto LastIndexOfAny(StringView charSet) const noexcept -> size_type {
        const const_pointer searchStart = ptr + len;
        for (const_pointer i = searchStart; i != ptr; --i) {
            if (charSet.Contains(*(i - 1)))
                return static_cast<size_type>(i - ptr - 1);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Find the last occurrence of any character in the specified string in this StringView.
    ///
    /// @param charSet  The string containing the characters to be searched in this StringView.
    /// @param from     The index to start searching from.
    ///
    /// @return size_type
    ///   Return the index of the last occurrence of any character in the specified string if exists. Otherwise, return -1.
    YAGE_NODISCARD constexpr auto LastIndexOfAny(StringView charSet, size_type from) const noexcept -> size_type {
        const const_pointer searchStart = (from + 1 > len ? ptr + len : ptr + from + 1);
        for (const_pointer i = searchStart; i != ptr; --i) {
            if (charSet.Contains(*i))
                return static_cast<size_type>(i - ptr - 1);
        }
        return size_type(-1);
    }

    /// @brief
    ///   Remove all leading characters occured in the specified character set.
    ///
    /// @param charSet  The string containing the characters to be removed.
    ///
    /// @return StringView &
    ///   Return reference to this StringView.
    constexpr auto TrimStart(StringView charSet = u" \f\n\r\t\v") noexcept -> StringView & {
        while (len && charSet.Contains(*ptr)) {
            ++ptr;
            --len;
        }
        return *this;
    }

    /// @brief
    ///   Remove all trailing characters occured in the specified character set.
    ///
    /// @param charSet  The string containing the characters to be removed.
    ///
    /// @return StringView &
    ///   Return reference to this StringView.
    constexpr auto TrimEnd(StringView charSet = u" \f\n\r\t\v") noexcept -> StringView & {
        while (len && charSet.Contains(ptr[len - 1]))
            --len;
        return *this;
    }

    /// @brief
    ///   Remove all leading and trailing characters occured in the specified character set.
    ///
    /// @param charSet  The string containing the characters to be removed.
    ///
    /// @return StringView &
    ///   Return reference to this StringView.
    constexpr auto Trim(StringView charSet = u" \f\n\r\t\v") noexcept -> StringView & {
        return TrimStart(charSet).TrimEnd(charSet);
    }

    /// @brief
    ///   Split this StringView into substrings using the specified delimiters.
    ///
    /// @param delims   The string containing the delimiters.
    ///
    /// @return std::vector<StringView>
    ///   Return an array of StringView containing the substrings.
    YAGE_NODISCARD YAGE_API auto Split(StringView delims) const noexcept -> std::vector<StringView>;

    /// @brief
    ///   Split this StringView into substrings using the specified delimiter.
    ///
    /// @param delim    The delimiter character.
    ///
    /// @return std::vector<StringView>
    ///   Return an array of StringView containing the substrings.
    YAGE_NODISCARD YAGE_API auto Split(value_type delim) const noexcept -> std::vector<StringView>;

    /// @brief
    ///   Calculate hash value of this StringView.
    ///
    /// @return size_t
    ///   Return hash value of this StringView.
    YAGE_NODISCARD YAGE_FORCEINLINE auto Hash() const noexcept -> size_t {
        return YaGE::Hash(ptr, len * sizeof(value_type), len);
    }

private:
    /// @brief Pointer to start of this string.
    const value_type *ptr = nullptr;

    /// @brief Number of char16_t characters in this string.
    size_type len = 0;
};

YAGE_CONSTEXPR17 auto operator==(StringView lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) == 0; }
YAGE_CONSTEXPR17 auto operator!=(StringView lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) != 0; }
YAGE_CONSTEXPR17 auto operator<(StringView lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) < 0; }
YAGE_CONSTEXPR17 auto operator<=(StringView lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) <= 0; }
YAGE_CONSTEXPR17 auto operator>(StringView lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) > 0; }
YAGE_CONSTEXPR17 auto operator>=(StringView lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) >= 0; }
YAGE_CONSTEXPR17 auto operator==(StringView lhs, const char16_t *rhs) noexcept -> bool { return lhs.Compare(rhs) == 0; }
YAGE_CONSTEXPR17 auto operator!=(StringView lhs, const char16_t *rhs) noexcept -> bool { return lhs.Compare(rhs) != 0; }
YAGE_CONSTEXPR17 auto operator<(StringView lhs, const char16_t *rhs) noexcept -> bool { return lhs.Compare(rhs) < 0; }
YAGE_CONSTEXPR17 auto operator<=(StringView lhs, const char16_t *rhs) noexcept -> bool { return lhs.Compare(rhs) <= 0; }
YAGE_CONSTEXPR17 auto operator>(StringView lhs, const char16_t *rhs) noexcept -> bool { return lhs.Compare(rhs) > 0; }
YAGE_CONSTEXPR17 auto operator>=(StringView lhs, const char16_t *rhs) noexcept -> bool { return lhs.Compare(rhs) >= 0; }

YAGE_CONSTEXPR17 auto operator==(const char16_t *lhs, StringView rhs) noexcept -> bool {
    return StringView(lhs) == rhs;
}

YAGE_CONSTEXPR17 auto operator!=(const char16_t *lhs, StringView rhs) noexcept -> bool {
    return StringView(lhs) != rhs;
}

YAGE_CONSTEXPR17 auto operator<(const char16_t *lhs, StringView rhs) noexcept -> bool { return StringView(lhs) < rhs; }

YAGE_CONSTEXPR17 auto operator<=(const char16_t *lhs, StringView rhs) noexcept -> bool {
    return StringView(lhs) <= rhs;
}

YAGE_CONSTEXPR17 auto operator>(const char16_t *lhs, StringView rhs) noexcept -> bool { return StringView(lhs) > rhs; }

YAGE_CONSTEXPR17 auto operator>=(const char16_t *lhs, StringView rhs) noexcept -> bool {
    return StringView(lhs) >= rhs;
}

} // namespace YaGE

template <>
struct std::hash<YaGE::StringView> {
    auto operator()(YaGE::StringView str) const noexcept -> size_t { return str.Hash(); }
};

template <>
struct fmt::formatter<YaGE::StringView, char16_t> : fmt::formatter<fmt::basic_string_view<char16_t>, char16_t> {
    template <typename FormatContext>
    auto format(YaGE::StringView str, FormatContext &ctx) const -> decltype(ctx.out()) {
        return fmt::formatter<fmt::basic_string_view<char16_t>, char16_t>::format({str.Data(), str.Length()}, ctx);
    }
};

#endif // YAGE_CORE_STRING_VIEW_H
