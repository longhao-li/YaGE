#pragma once

#include "StringView.h"

namespace YaGE {

class String {
public:
    using traits_type     = std::char_traits<char16_t>;
    using value_type      = char16_t;
    using pointer         = value_type *;
    using const_pointer   = const value_type *;
    using reference       = value_type &;
    using const_reference = const value_type &;
    using iterator        = pointer;
    using const_iterator  = const_pointer;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

private:
    struct LongStr {
        pointer   ptr;
        size_type capacity;
        size_type size;
        size_type padding : sizeof(size_type) * 8 - 1;
        size_type isLong : 1;
    };

    static constexpr const size_type SHORT_CAPACITY = (sizeof(LongStr) - 1) / sizeof(value_type);

    struct ShortStr {
        value_type str[SHORT_CAPACITY];
        uint8_t    size;
        uint8_t    padding : 7;
        uint8_t    isLong  : 1;
    };

public:
    /// @brief
    ///   Create an empty string.
    String() noexcept : _impl() {}

    /// @brief
    ///   Create a string of a consecutive sequence of characters.
    ///
    /// @param[in] ptr  Pointer to start of the character sequence.
    /// @param     len  Number of characters in the sequence.
    YAGE_API String(const value_type *ptr, size_type len) noexcept;

    /// @brief
    ///   Create a string from a C-style string.
    ///
    /// @param[in] ptr  Pointer to start of the C-style string.
    YAGE_API String(const value_type *ptr) noexcept;

    /// @brief
    ///   Create a string from StringView.
    ///
    /// @param str  The StringView to be copied from.
    YAGE_API String(StringView str) noexcept;

    /// @brief
    ///   Copy constructor.
    ///
    /// @param[in] other    The string to be copied from.
    YAGE_API String(const String &other) noexcept;

    /// @brief
    ///   Copy assignmenet.
    ///
    /// @param[in] other    The string to be copied from.
    ///
    /// @return String &
    ///   Return reference to this String.
    YAGE_API auto operator=(const String &other) noexcept -> String &;

    /// @brief
    ///   Move constructor.
    ///
    /// @param other    The string to be moved.
    String(String &&other) noexcept {
        memcpy(&_impl, &other._impl, sizeof(_impl));
        memset(&other._impl, 0, sizeof(other._impl));
    }

    /// @brief
    ///   Move assignment.
    ///
    /// @param other    The string to be moved.
    ///
    /// @return String &
    ///   Return reference to this String.
    YAGE_API auto operator=(String &&other) noexcept -> String &;

    /// @brief
    ///   Constructing String from @p nullptr is disabled.
    String(std::nullptr_t) = delete;

    /// @brief
    ///   Destroy this String.
    YAGE_API ~String() noexcept;

    /// @brief
    ///   Assign a C-style string to this object.
    ///
    /// @param[in] str  The C-style string to be copied from.
    ///
    /// @return String &
    ///   Return reference to this String.
    YAGE_API auto operator=(const_pointer str) noexcept -> String &;

    /// @brief
    ///   Assign a character to this String.
    ///
    /// @param ch       The character to be assigned.
    ///
    /// @return String &
    ///   Return reference to this String.
    YAGE_API auto operator=(value_type ch) noexcept -> String &;

    /// @brief
    ///   Assign another string to this object.
    ///
    /// @param str      The string to be copied from.
    ///
    /// @return String &
    ///   Return reference to this String.
    YAGE_API auto operator=(StringView str) noexcept -> String &;

    /// @brief
    ///   Assign another string to this object.
    ///
    /// @param str      The string to be copied from.
    ///
    /// @return String &
    ///   Return reference to this String.
    auto Assign(const String &str) noexcept -> String & {
        *this = str;
        return *this;
    }

    /// @brief
    ///   Assign another string to this object.
    ///
    /// @param str      The string to be copied from.
    ///
    /// @return String &
    ///   Return reference to this String.
    auto Assign(String &&str) noexcept -> String & {
        *this = std::move(str);
        return *this;
    }

    /// @brief
    ///   Assign another string to this object.
    ///
    /// @param str      The string to be copied from.
    ///
    /// @return String &
    ///   Return reference to this String.
    auto Assign(StringView str) noexcept -> String & {
        *this = str;
        return *this;
    }

    /// @brief
    ///   Assign a C-style string to this object.
    ///
    /// @param str      The C-style string to be copied from.
    ///
    /// @return String &
    ///   Return reference to this String.
    auto Assign(const_pointer str) noexcept -> String & {
        *this = str;
        return *this;
    }

    /// @brief
    ///   Replace content of this string with a character sequence.
    ///
    /// @param[in] str  Pointer to start of the character sequence.
    /// @param     len  Number of characters in the sequence.
    ///
    /// @return String &
    ///   Return reference to this String.
    auto Assign(const_pointer str, size_type len) noexcept -> String & {
        *this = StringView(str, len);
        return *this;
    }

    /// @brief
    ///   Access character by index.
    ///
    /// @param index    Index of the character to be accessed.
    ///
    /// @return reference
    ///   Return reference to the specified character to be accessed.
    YAGE_NODISCARD auto operator[](size_type index) noexcept -> reference { return Data()[index]; }

    /// @brief
    ///   Access character by index.
    ///
    /// @param index    Index of the character to be accessed.
    ///
    /// @return const_reference
    ///   Return reference to the specified character to be accessed.
    YAGE_NODISCARD auto operator[](size_type index) const noexcept -> const_reference { return Data()[index]; }

    /// @brief
    ///   Get the first character in this string. Empty string check is not performed.
    ///
    /// @return reference
    ///   Return reference to the first character in this string.
    YAGE_NODISCARD auto Front() noexcept -> reference { return *Data(); }

    /// @brief
    ///   Get the first character in this string. Empty string check is not performed.
    ///
    /// @return const_reference
    ///   Return reference to the first character in this string.
    YAGE_NODISCARD auto Front() const noexcept -> const_reference { return *Data(); }

    /// @brief
    ///   Get the last character in this string. Empty string check is not performed.
    ///
    /// @return reference
    ///   Return reference to the last character in this string.
    YAGE_NODISCARD auto Back() noexcept -> reference { return *(Data() + Length() - 1); }

    /// @brief
    ///   Get the last character in this string. Empty string check is not performed.
    ///
    /// @return const_reference
    ///   Return reference to the last character in this string.
    YAGE_NODISCARD auto Back() const noexcept -> const_reference { return *(Data() + Length() - 1); }

    /// @brief
    ///   Get pointer to the beginning of this string.
    /// @remarks
    ///   It is guaranteed that this string is null-terminated, but there may be null characters in the middle of this string.
    ///
    /// @return pointer
    ///   Return pointer to the beginning of this string.
    YAGE_NODISCARD auto Data() noexcept -> pointer { return (_impl.l.isLong ? _impl.l.ptr : _impl.s.str); }

    /// @brief
    ///   Get pointer to the beginning of this string.
    /// @remarks
    ///   It is guaranteed that this string is null-terminated, but there may be null characters in the middle of this string.
    ///
    /// @return pointer
    ///   Return pointer to the beginning of this string.
    YAGE_NODISCARD auto Data() const noexcept -> const_pointer { return (_impl.l.isLong ? _impl.l.ptr : _impl.s.str); }

    /// @brief
    ///   Allow implicit conversion to StringView.
    operator StringView() const noexcept {
        if (_impl.l.isLong)
            return {_impl.l.ptr, _impl.l.size};
        else
            return {_impl.s.str, _impl.s.size};
    }

    /// @brief
    ///   Get iterator to the first character in this string.
    ///
    /// @return iterator
    ///   Return iterator to the first character in this string.
    YAGE_NODISCARD auto begin() noexcept -> iterator { return Data(); }

    /// @brief
    ///   Get iterator to the first character in this string.
    ///
    /// @return const_iterator
    ///   Return iterator to the first character in this string.
    YAGE_NODISCARD auto begin() const noexcept -> const_iterator { return Data(); }

    /// @brief
    ///   Get iterator to the position after the last character in this string.
    ///
    /// @return iterator
    ///   Return iterator to the position after the last character in this string.
    YAGE_NODISCARD auto end() noexcept -> iterator { return Data() + Length(); }

    /// @brief
    ///   Get iterator to the position after the last character in this string.
    ///
    /// @return const_iterator
    ///   Return iterator to the position after the last character in this string.
    YAGE_NODISCARD auto end() const noexcept -> const_iterator { return Data() + Length(); }

    /// @brief
    ///   Checks if this string is empty.
    ///
    /// @return bool
    /// @retval true    This is an empty string.
    /// @retval false   This is not an empty string.
    YAGE_NODISCARD auto IsEmpty() const noexcept -> bool { return Length() == 0; }

    /// @brief
    ///   Get number of characters in this string.
    ///
    /// @return size_type
    ///   Return number of characters in this string.
    YAGE_NODISCARD auto Length() const noexcept -> size_type { return (_impl.l.isLong ? _impl.l.size : _impl.s.size); }

    /// @brief
    ///   Reserve at least @p count characters space in this string.
    ///
    /// @param count    Expected number of characters of storage in this string to be reserved.
    YAGE_API auto Reserve(size_type count) noexcept -> void;

    /// @brief
    ///   Get number of characters that this string has currently allocated space for.
    ///
    /// @return size_type
    ///   Return number of characters that this string has currently allocated space for.
    YAGE_NODISCARD auto Capacity() const noexcept -> size_type {
        return (_impl.l.isLong ? _impl.l.capacity : (SHORT_CAPACITY - 1));
    }

    /// @brief
    ///   Remove all characters in this string.
    YAGE_API auto Clear() noexcept -> void;

    /// @brief
    ///   Insert a string at the specified position of this string.
    /// @note
    ///   This method is equivalent to @p Append if @p position is greater than length of this string.
    ///
    /// @param     position     Index of the character position to insert the new string.
    /// @param[in] str          The string to be inserted.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto Insert(size_type position, StringView str) noexcept -> String &;

    /// @brief
    ///   Insert a sequence of characters at the specified position of this string.
    /// @note
    ///   This method is equivalent to @p Append if @p position is greater than length of this string.
    ///
    /// @param     position     Index of the character position to insert the new string.
    /// @param[in] str          The string to be inserted.
    /// @param     count        Number of characters to be inserted.
    ///
    /// @return String &
    ///   Return reference to this string.
    auto Insert(size_type position, const_pointer str, size_type count) noexcept -> String & {
        return Insert(position, StringView(str, count));
    }

    /// @brief
    ///   Insert @p count characters at the specified position of this string.
    /// @note
    ///   This method is equivalent to @p Append if @p position is greater than length of this string.
    ///
    /// @param position     Index of the character position to insert the new characters.
    /// @param count        Number of characters to be copied.
    /// @param ch           The character to be copied.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto Insert(size_type position, size_type count, value_type ch) noexcept -> String &;

    /// @brief
    ///   Remove @p count characters from the specified index.
    ///
    /// @param from     Index of the character to start remove.
    /// @param count    Number of characters to be removed.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto Remove(size_type from, size_type count) noexcept -> String &;

    /// @brief
    ///   Remove all characters between [first, last).
    ///
    /// @param first    Iterator to the first character to be removed.
    /// @param last     Iterator to the position after the last character to be removed.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto Remove(const_iterator first, const_iterator last) noexcept -> String &;

    /// @brief
    ///   Remove all characters after the specified index (included).
    ///
    /// @param index    Index to start of the characters to be removed.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto RemoveAfter(size_type index) noexcept -> String &;

    /// @brief
    ///   Append the given character to the end of this string.
    ///
    /// @param ch       The character to be appended.
    YAGE_API auto push_back(value_type ch) noexcept -> void;

    /// @brief
    ///   Remove the last character in this string. Empty check is performed.
    YAGE_API auto pop_back() noexcept -> void;

    /// @brief
    ///   Append @p count copies of character @p ch at the end of this string.
    ///
    /// @param count    Number of the specified characters to be copied.
    /// @param ch       The character to be copied.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto Append(size_type count, value_type ch) noexcept -> String &;

    /// @brief
    ///   Append a sequence of characters at the end of this string.
    ///
    /// @param[in] str      Pointer to start of the character sequence to be appended.
    /// @param     count    Number of characters in the sequence.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto Append(const value_type *str, size_type count) noexcept -> String &;

    /// @brief
    ///   Append the specified string to the end of this string.
    ///
    /// @param str      The string to be appended.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto Append(StringView str) noexcept -> String & { return Append(str.Data(), str.Length()); }

    /// @brief
    ///   Append the given character to the end of this string.
    ///
    /// @param ch       The character to be appended.
    ///
    /// @return String &
    ///   Return reference to this string.
    auto operator+=(value_type ch) noexcept -> String & {
        push_back(ch);
        return *this;
    }

    /// @brief
    ///   Append a C-style string to the end of this string.
    ///
    /// @param[in] str  The C-style string to be appended.
    ///
    /// @return String &
    ///   Return reference to this string.
    auto operator+=(const value_type *str) noexcept -> String & { return Append(str, traits_type::length(str)); }

    /// @brief
    ///   Append another string to the end of this string.
    ///
    /// @param[in] str  The string to be appended.
    ///
    /// return String &
    ///   Return reference to this string.
    auto operator+=(StringView str) noexcept -> String & { return Append(str.Data(), str.Length()); }

    /// @brief
    ///   Append another string to the end of this string.
    ///
    /// @param[in] str  The string to be appended.
    ///
    /// return String &
    ///   Return reference to this string.
    auto operator+=(const String &str) noexcept -> String & { return Append(str.Data(), str.Length()); }

    /// @brief
    ///   Compare two strings lexically.
    ///
    /// @param rhs      Another string to be compared with.
    ///
    /// @return int
    /// @retval -1      This string is lexically less than @p rhs.
    /// @retval 0       This string is exactly the same as @p rhs.
    /// @retval 1       This string is lexically greater than @p rhs.
    YAGE_NODISCARD auto Compare(StringView str) const noexcept -> int { return StringView(*this).Compare(str); }

    /// @brief
    ///   Checks if this string starts with the specified string.
    ///
    /// @param str      The string to be compared with start of this string.
    ///
    /// @return bool
    /// @retval true    Target string @p str matches start part of this string.
    /// @retval false   Target string @p str doesn't match start part of this string.
    YAGE_NODISCARD auto StartsWith(StringView str) const noexcept -> bool { return StringView(*this).StartsWith(str); }

    /// @brief
    ///   Checks if this string starts with the specified character.
    ///
    /// @param ch       The character to be compared with the first character in this string.
    ///
    /// @return bool
    /// @retval true    This string starts with character @p ch.
    /// @retval false   This string doesn't start with character @p ch.
    YAGE_NODISCARD auto StartsWith(value_type ch) const noexcept -> bool { return StringView(*this).StartsWith(ch); }

    /// @brief
    ///   Checks if this string ends with the specified string.
    ///
    /// @param str      The string to be compared with end of this string.
    ///
    /// @return bool
    /// @retval true    Target string @p str matches end part of this string.
    /// @retval false   Target string @p str doesn't match end part of this string.
    YAGE_NODISCARD auto EndsWith(StringView str) const noexcept -> bool { return StringView(*this).EndsWith(str); }

    /// @brief
    ///   Checks if this string ends with the specified character.
    ///
    /// @param ch       The character to be compared with the last character in this string.
    ///
    /// @return bool
    /// @retval true    This string ends with character @p ch.
    /// @retval false   This string doesn't end with character @p ch.
    YAGE_NODISCARD auto EndsWith(value_type ch) const noexcept -> bool { return StringView(*this).EndsWith(ch); }

    /// @brief
    ///   Checks if this string contains the specified sub-string.
    ///
    /// @param str      The string pattern to be found.
    ///
    /// @return bool
    /// @retval true    The string pattern @p str is contained in this string.
    /// @retval false   The string pattern @p str is not contained in this string.
    YAGE_NODISCARD auto Contains(StringView str) const noexcept -> bool { return StringView(*this).Contains(str); }

    /// @brief
    ///   Checks if this string contains the specified character.
    ///
    /// @param ch       The character to be found in this string.
    ///
    /// @return bool
    /// @retval true    The character @p ch is contained in this string.
    /// @retval false   The character @p ch is not contained in this string.
    YAGE_NODISCARD auto Contains(value_type ch) const noexcept -> bool { return StringView(*this).Contains(ch); }

    /// @brief
    ///   Create a sub-string from the @p from'th character to end of this string.
    ///
    /// @param from     Index of start character of the sub-string.
    ///
    /// @return StringView
    ///   Return a new StringView. It is guaranteed that the new StringView is a valid string.
    YAGE_NODISCARD auto SubString(size_type from) const noexcept -> StringView {
        return StringView(*this).SubString(from);
    }

    /// @brief
    ///   Create a sub-string that contains @p count characters from @p from'th character in this
    ///   string.
    ///
    /// @param from     Index of start character of the sub-string.
    /// @param count    Number of characters in the sub-string.
    ///
    /// @return StringView
    ///   Return a new StringView. It is guaranteed that the new StringView is a valid string.
    YAGE_NODISCARD auto SubString(size_type from, size_type count) const noexcept -> StringView {
        return StringView(*this).SubString(from, count);
    }

    /// @brief
    ///   Copy part of this string to the specified memory.
    ///
    /// @param[out] dest    Pointer to the copy destination.
    /// @param      count   Expected number of characters to be copied. This value may be clamped if is greater than maximum copyable character count.
    /// @param      from    Start index of this string to copy characters.
    ///
    /// @return size_type
    ///   Return number of characters copied.
    YAGE_NODISCARD auto CopyTo(pointer dest, size_type count, size_type from = 0) const noexcept -> size_type {
        return StringView(*this).CopyTo(dest, count, from);
    }

    /// @brief
    ///   Resize this string to contain @p count characters.
    ///
    /// @param count    New size of this string.
    /// @param ch       Character to initialize the new characters with.
    YAGE_API auto Resize(size_type count, value_type ch = value_type()) noexcept -> void;

    /// @brief
    ///   Get start index of the specified string pattern.
    ///
    /// @param str      The string pattern to be found.
    /// @param startPos Start index of this string to search for the specified pattern.
    ///
    /// @return size_type
    ///   Return index to start of the specified string pattern if exists. Return -1 if no such string pattern exists.
    YAGE_NODISCARD auto IndexOf(StringView str, size_type startPos = 0) const noexcept -> size_type {
        return StringView(*this).IndexOf(str, startPos);
    }

    /// @brief
    ///   Find the first occurance index of the specified character from @p startPos.
    ///
    /// @param ch           The character to be found in this string.
    /// @param startPos     Index to start searching for the specified character.
    ///
    /// @return size_type
    ///   Return index of the specified character if found. Return -1 if no such character found.
    YAGE_NODISCARD auto IndexOf(value_type ch, size_type startPos = 0) const noexcept -> size_type {
        return StringView(*this).IndexOf(ch, startPos);
    }

    /// @brief
    ///   Find index of the last occurance of the specified string pattern in this string.
    ///
    /// @param str      The string pattern to be found in this string.
    ///
    /// @return size_type
    ///   Return index of the specified string pattern if found in this string. Return -1 if no such string pattern found.
    YAGE_NODISCARD auto LastIndexOf(StringView str) const noexcept -> size_type {
        return StringView(*this).LastIndexOf(str);
    }

    /// @brief
    ///   Find index of the last occurance of the specified string pattern in this string.
    ///
    /// @param str      The string pattern to be found in this string.
    /// @param startPos Index to start searching for the specified string pattern.
    ///
    /// @return size_type
    ///   Return index of the specified string pattern if found in this string. Return -1 if no such string pattern found.
    YAGE_NODISCARD auto LastIndexOf(StringView str, size_type startPos) const noexcept -> size_type {
        return StringView(*this).LastIndexOf(str, startPos);
    }

    /// @brief
    ///   Find the last occurance of the specified character in this string.
    ///
    /// @param ch   The character to be found in this string.
    ///
    /// @return size_type
    ///   Return index of the specified character if found. Return -1 if no such character is found in this string.
    YAGE_NODISCARD auto LastIndexOf(value_type ch) const noexcept -> size_type {
        return StringView(*this).LastIndexOf(ch);
    }

    /// @brief
    ///   Find the last occurance of the specified character from @p startPos of this string.
    ///
    /// @param ch       The character to be found in this string.
    /// @param startPos Index to start searching for the specified character.
    ///
    /// @return size_type
    ///   Return index of the specified character if found. Return -1 if no such character is found in this string.
    YAGE_NODISCARD auto LastIndexOf(value_type ch, size_type startPos) const noexcept -> size_type {
        return StringView(*this).LastIndexOf(ch, startPos);
    }

    /// @brief
    ///   Get the index of the first occurrence in this string of any character in the specified character set.
    ///
    /// @param charSet  A set of characters to be found.
    /// @param startPos Position to start searching for the specified characters.
    ///
    /// @return size_type
    ///   Return index to the character that first occured in the specified character set. Return -1 if no such character found.
    YAGE_NODISCARD auto IndexOfAny(StringView charSet, size_type startPos = 0) const noexcept -> size_type {
        return StringView(*this).IndexOfAny(charSet, startPos);
    }

    /// @brief
    ///   Get the index of the last occurrence in this string of any character in the specified character set.
    ///
    /// @param charSet  A set of characters to be found.
    ///
    /// @return size_type
    ///   Return index to the character that last occured in the specified character set. Return -1 if no such character found.
    YAGE_NODISCARD auto LastIndexOfAny(StringView charSet) const noexcept -> size_type {
        return StringView(*this).LastIndexOfAny(charSet);
    }

    /// @brief
    ///   Get the index of the last occurrence in this StringView of any character in the specified character set.
    ///
    /// @param charSet  A set of characters to be found.
    /// @param startPos Index to start searching for the specified characters.
    ///
    /// @return size_type
    ///   Return index to the character that last occured in the specified character set. Return -1 if no such character found.
    YAGE_NODISCARD auto LastIndexOfAny(StringView charSet, size_type startPos) const noexcept -> size_type {
        return StringView(*this).LastIndexOfAny(charSet, startPos);
    }

    /// @brief
    ///   Remove all leading characters occured in @p charSet.
    ///
    /// @param charSet  A set of characters to be removed.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto TrimStart(StringView charSet = u" \f\n\r\t\v") noexcept -> String &;

    /// @brief
    ///   Remove all trailing characters occured in @p charSet.
    ///
    /// @param charSet  A set of characters to be removed.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto TrimEnd(StringView charSet = u" \f\n\r\t\v") noexcept -> String &;

    /// @brief
    ///   Remove all leading and trailing characters occured in @p charSet.
    ///
    /// @param charSet  A set of characters to be removed.
    ///
    /// @return String &
    ///   Return reference to this string.
    auto Trim(StringView charSet = u" \f\n\r\t\v") noexcept -> String & {
        TrimEnd(charSet);
        TrimStart(charSet);
        return *this;
    }

    /// @brief
    ///   Pad beginning of this string with @p count characters @p ch.
    ///
    /// @param count    Number of characters to be padded.
    /// @param ch       The character that is used to pad.
    ///
    /// @return String &
    ///   Return reference to this string.
    auto PadLeft(size_type count, value_type ch = u' ') noexcept -> String & { return Insert(0, count, ch); }

    /// @brief
    ///   Pad end of this string with @p count characters @p ch.
    ///
    /// @param count    Number of characters to be padded.
    /// @param ch       The character that is used to pad.
    ///
    /// @return String &
    ///   Return reference to this string.
    auto PadRight(size_type count, value_type ch = ' ') noexcept -> String & { return Append(count, ch); }

    /// @brief
    ///   Convert all English alphabets to lowercase.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto ToLower() noexcept -> String &;

    /// @brief
    ///   Convert all English alphabets to uppercase.
    ///
    /// @return String &
    ///   Return reference to this string.
    YAGE_API auto ToUpper() noexcept -> String &;

    /// @brief
    ///   Calculate hash value of this string.
    ///
    /// @return size_t
    ///   Return hash value of this string.
    YAGE_NODISCARD auto Hash() const noexcept -> size_t {
        if (_impl.l.isLong)
            return YaGE::Hash(_impl.l.ptr, _impl.l.size * sizeof(value_type), _impl.l.size);
        else
            return YaGE::Hash(_impl.s.str, _impl.s.size * sizeof(value_type), _impl.s.size);
    }

private:
    union {
        LongStr  l;
        ShortStr s;
    } _impl;
};

YAGE_FORCEINLINE auto operator==(const String &lhs, const String &rhs) noexcept -> bool {
    return lhs.Compare(rhs) == 0;
}

YAGE_FORCEINLINE auto operator!=(const String &lhs, const String &rhs) noexcept -> bool {
    return lhs.Compare(rhs) != 0;
}

YAGE_FORCEINLINE auto operator<(const String &lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) < 0; }

YAGE_FORCEINLINE auto operator<=(const String &lhs, const String &rhs) noexcept -> bool {
    return lhs.Compare(rhs) <= 0;
}

YAGE_FORCEINLINE auto operator>(const String &lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) > 0; }

YAGE_FORCEINLINE auto operator>=(const String &lhs, const String &rhs) noexcept -> bool {
    return lhs.Compare(rhs) >= 0;
}

YAGE_FORCEINLINE auto operator==(const String &lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) == 0; }
YAGE_FORCEINLINE auto operator!=(const String &lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) != 0; }
YAGE_FORCEINLINE auto operator<(const String &lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) < 0; }
YAGE_FORCEINLINE auto operator<=(const String &lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) <= 0; }
YAGE_FORCEINLINE auto operator>(const String &lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) > 0; }
YAGE_FORCEINLINE auto operator>=(const String &lhs, StringView rhs) noexcept -> bool { return lhs.Compare(rhs) >= 0; }

YAGE_FORCEINLINE auto operator==(StringView lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) == 0; }
YAGE_FORCEINLINE auto operator!=(StringView lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) != 0; }
YAGE_FORCEINLINE auto operator<(StringView lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) < 0; }
YAGE_FORCEINLINE auto operator<=(StringView lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) <= 0; }
YAGE_FORCEINLINE auto operator>(StringView lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) > 0; }
YAGE_FORCEINLINE auto operator>=(StringView lhs, const String &rhs) noexcept -> bool { return lhs.Compare(rhs) >= 0; }

YAGE_FORCEINLINE auto operator==(const String &lhs, const char16_t *rhs) noexcept -> bool {
    return lhs.Compare(rhs) == 0;
}

YAGE_FORCEINLINE auto operator!=(const String &lhs, const char16_t *rhs) noexcept -> bool {
    return lhs.Compare(rhs) != 0;
}

YAGE_FORCEINLINE auto operator<(const String &lhs, const char16_t *rhs) noexcept -> bool {
    return lhs.Compare(rhs) < 0;
}

YAGE_FORCEINLINE auto operator<=(const String &lhs, const char16_t *rhs) noexcept -> bool {
    return lhs.Compare(rhs) <= 0;
}

YAGE_FORCEINLINE auto operator>(const String &lhs, const char16_t *rhs) noexcept -> bool {
    return lhs.Compare(rhs) > 0;
}

YAGE_FORCEINLINE auto operator>=(const String &lhs, const char16_t *rhs) noexcept -> bool {
    return lhs.Compare(rhs) >= 0;
}

YAGE_FORCEINLINE auto operator==(const char16_t *lhs, const String &rhs) noexcept -> bool {
    return StringView(lhs).Compare(rhs) == 0;
}

YAGE_FORCEINLINE auto operator!=(const char16_t *lhs, const String &rhs) noexcept -> bool {
    return StringView(lhs).Compare(rhs) != 0;
}

YAGE_FORCEINLINE auto operator<(const char16_t *lhs, const String &rhs) noexcept -> bool {
    return StringView(lhs).Compare(rhs) < 0;
}

YAGE_FORCEINLINE auto operator<=(const char16_t *lhs, const String &rhs) noexcept -> bool {
    return StringView(lhs).Compare(rhs) <= 0;
}

YAGE_FORCEINLINE auto operator>(const char16_t *lhs, const String &rhs) noexcept -> bool {
    return StringView(lhs).Compare(rhs) > 0;
}

YAGE_FORCEINLINE auto operator>=(const char16_t *lhs, const String &rhs) noexcept -> bool {
    return StringView(lhs).Compare(rhs) >= 0;
}

YAGE_API auto operator+(const String &lhs, const String &rhs) noexcept -> String;
YAGE_API auto operator+(const String &lhs, StringView rhs) noexcept -> String;
YAGE_API auto operator+(StringView lhs, const String &rhs) noexcept -> String;
YAGE_API auto operator+(const String &lhs, const char16_t *rhs) noexcept -> String;
YAGE_API auto operator+(const char16_t *lhs, const String &rhs) noexcept -> String;

/// @brief
///   Format string with compile-time argument check.
template <typename... Args>
using FormatString = fmt::basic_format_string<char16_t, fmt::type_identity_t<Args>...>;

/// @brief
///   Format context for string.
using FormatContext = fmt::buffer_context<char16_t>;

/// @brief
///   Format a string and write the output to @p out.
///
/// @param out      The output iterator.
/// @param fmt      The format pattern string.
/// @param args     The arguments to be formatted.
///
/// @return OutputIt
///   Return the output iterator to end of the formatted context.
template <typename OutputIt,
          typename... T,
          typename = std::enable_if_t<fmt::detail::is_output_iterator<OutputIt, char16_t>::value>>
auto FormatTo(OutputIt out, FormatString<T...> fmt, T &&...args) -> OutputIt {
    auto &&buf = fmt::detail::get_buffer<char16_t>(out);
    fmt::detail::vformat_to(buf, fmt::basic_string_view<char16_t>(fmt),
                            fmt::make_format_args<FormatContext>(std::forward<T>(args)...));
    return fmt::detail::get_iterator(buf);
}

/// @brief
///   Format string with arguments.
///
/// @param format   The format pattern string.
/// @param args     The arguments to format.
///
/// @return String
///   Return the formatted string.
template <typename... Args>
YAGE_NODISCARD auto Format(FormatString<Args...> format, Args &&...args) -> String {
    auto formatArgs{fmt::make_format_args<FormatContext>(std::forward<Args>(args)...)};
    auto buffer{fmt::basic_memory_buffer<char16_t>()};
    fmt::detail::vformat_to(buffer, fmt::basic_string_view<char16_t>(format), std::move(formatArgs));
    return {buffer.data(), buffer.size()};
}

} // namespace YaGE

template <>
struct std::hash<YaGE::String> {
    auto operator()(const YaGE::String &str) const noexcept -> size_t { return str.Hash(); }
};

template <>
struct fmt::formatter<YaGE::String, char16_t> : fmt::formatter<fmt::basic_string_view<char16_t>, char16_t> {
    template <typename FormatContext>
    auto format(const YaGE::String &str, FormatContext &ctx) const -> decltype(ctx.out()) {
        return fmt::formatter<fmt::basic_string_view<char16_t>, char16_t>::format({str.Data(), str.Length()}, ctx);
    }
};
