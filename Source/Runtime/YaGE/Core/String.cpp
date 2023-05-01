#include "String.h"

using namespace YaGE;

static inline auto alignUpAllocCount(size_t cap) noexcept -> size_t {
    constexpr const size_t alignment = sizeof(void *);
    return ((cap + alignment - 1) & ~size_t(alignment - 1));
}

#if !defined(__clang__) && defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 6011 6308 6385 6386 6387)
#endif

YaGE::String::String(const value_type *ptr, size_type len) noexcept {
    if (len < SHORT_CAPACITY) {
        traits_type::copy(_impl.s.str, ptr, len);
        traits_type::assign(_impl.s.str[len], value_type());
        _impl.s.size    = static_cast<uint8_t>(len);
        _impl.s.padding = 0;
        _impl.s.isLong  = 0;
    } else {
        const size_type allocCount = alignUpAllocCount(len + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        traits_type::copy(buffer, ptr, len);
        traits_type::assign(buffer[len], value_type());

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = len;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }
}

YaGE::String::String(const value_type *ptr) noexcept {
    const size_type len = traits_type::length(ptr);
    if (len < SHORT_CAPACITY) {
        // Copy with trailing '\0'.
        traits_type::copy(_impl.s.str, ptr, len + 1);
        _impl.s.size    = static_cast<uint8_t>(len);
        _impl.s.padding = 0;
        _impl.s.isLong  = 0;
    } else {
        const size_type allocCount = alignUpAllocCount(len + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        // Copy with trailing '\0'.
        traits_type::copy(buffer, ptr, len + 1);

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = len;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }
}

YaGE::String::String(StringView str) noexcept {
    const size_type     len = str.Length();
    const const_pointer ptr = str.Data();

    if (len < SHORT_CAPACITY) {
        traits_type::copy(_impl.s.str, ptr, len);
        traits_type::assign(_impl.s.str[len], value_type());
        _impl.s.size    = static_cast<uint8_t>(len);
        _impl.s.padding = 0;
        _impl.s.isLong  = 0;
    } else {
        const size_type allocCount = alignUpAllocCount(len + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        traits_type::copy(buffer, ptr, len);
        traits_type::assign(buffer[len], value_type());

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = len;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }
}

YaGE::String::String(const String &other) noexcept {
    if (other._impl.l.isLong == 0) {
        // Copy if is short string.
        _impl = other._impl;
    } else {
        const const_pointer ptr = other._impl.l.ptr;
        const size_type     len = other._impl.l.size;

        const size_type allocCount = alignUpAllocCount(len + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        // Copy with trailing '\0'.
        traits_type::copy(buffer, ptr, len + 1);

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = len;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }
}

auto YaGE::String::operator=(const String &other) noexcept -> String & {
    // Self assign.
    if (this == &other)
        return *this;

    const_pointer str;
    size_type     len;

    if (other._impl.l.isLong) {
        str = other._impl.l.ptr;
        len = other._impl.l.size;
    } else {
        str = other._impl.s.str;
        len = other._impl.s.size;
    }

    if (len < SHORT_CAPACITY) {
        if (_impl.l.isLong) {
            traits_type::copy(_impl.l.ptr, str, len + 1);
            _impl.l.size = len;
        } else {
            traits_type::copy(_impl.s.str, str, len + 1);
            _impl.s.size = static_cast<uint8_t>(len);
        }

        return *this;
    }

    if (_impl.l.isLong) {
        if (_impl.l.capacity < len) {
            const size_type allocCount = alignUpAllocCount(len + 1);
            const size_type allocSize  = allocCount * sizeof(value_type);

            free(_impl.l.ptr);
            _impl.l.ptr      = static_cast<pointer>(malloc(allocSize));
            _impl.l.capacity = allocCount - 1;
        }

        traits_type::copy(_impl.l.ptr, str, len + 1);
        _impl.l.size = len;
    } else {
        const size_type allocCount = alignUpAllocCount(len + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        // Copy with trailing '\0'.
        traits_type::copy(buffer, str, len + 1);

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = len;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }

    return *this;
}

auto YaGE::String::operator=(String &&other) noexcept -> String & {
    if (_impl.l.isLong)
        free(_impl.l.ptr);

    _impl = other._impl;
    memset(&other._impl, 0, sizeof(other._impl));

    return *this;
}

YaGE::String::~String() noexcept {
    if (_impl.l.isLong)
        free(_impl.l.ptr);
}

auto YaGE::String::operator=(const_pointer str) noexcept -> String & {
    const size_type len = traits_type::length(str);
    if (len < SHORT_CAPACITY) {
        if (_impl.l.isLong) {
            traits_type::move(_impl.l.ptr, str, len + 1);
            _impl.l.size = len;
        } else {
            traits_type::move(_impl.s.str, str, len + 1);
            _impl.s.size = static_cast<uint8_t>(len);
        }

        return *this;
    }

    if (_impl.l.isLong) {
        if (_impl.l.capacity < len) {
            const size_type allocCount = alignUpAllocCount(len + 1);
            const size_type allocSize  = allocCount * sizeof(value_type);

            free(_impl.l.ptr);
            _impl.l.ptr      = static_cast<pointer>(malloc(allocSize));
            _impl.l.capacity = allocCount - 1;
        }

        traits_type::move(_impl.l.ptr, str, len + 1);
        _impl.l.size = len;
    } else {
        const size_type allocCount = alignUpAllocCount(len + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        // Copy with trailing '\0'.
        traits_type::move(buffer, str, len + 1);

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = len;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }

    return *this;
}

auto YaGE::String::operator=(value_type ch) noexcept -> String & {
    if (_impl.l.isLong) {
        traits_type::assign(_impl.l.ptr[0], ch);
        traits_type::assign(_impl.l.ptr[1], value_type());
        _impl.l.size = 1;
    } else {
        traits_type::assign(_impl.s.str[0], ch);
        traits_type::assign(_impl.s.str[1], value_type());
        _impl.s.size = 1;
    }

    return *this;
}

auto YaGE::String::operator=(StringView str) noexcept -> String & {
    const const_pointer ptr = str.Data();
    const size_type     len = str.Length();

    if (len < SHORT_CAPACITY) {
        if (_impl.l.isLong) {
            traits_type::move(_impl.l.ptr, ptr, len);
            traits_type::assign(_impl.l.ptr[len], value_type());
            _impl.l.size = len;
        } else {
            traits_type::move(_impl.s.str, ptr, len);
            traits_type::assign(_impl.l.ptr[len], value_type());
            _impl.s.size = static_cast<uint8_t>(len);
        }

        return *this;
    }

    if (_impl.l.isLong) {
        if (_impl.l.capacity < len) {
            const size_type allocCount = alignUpAllocCount(len + 1);
            const size_type allocSize  = allocCount * sizeof(value_type);

            free(_impl.l.ptr);
            _impl.l.ptr      = static_cast<pointer>(malloc(allocSize));
            _impl.l.capacity = allocCount - 1;
        }

        traits_type::move(_impl.l.ptr, ptr, len);
        traits_type::assign(_impl.l.ptr[len], value_type());
        _impl.l.size = len;
    } else {
        const size_type allocCount = alignUpAllocCount(len + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        traits_type::move(buffer, ptr, len);
        traits_type::assign(buffer[len], value_type());

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = len;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }

    return *this;
}

auto YaGE::String::Reserve(size_type newCap) noexcept -> void {
    if (_impl.l.isLong) {
        if (newCap <= _impl.l.capacity)
            return;

        const size_type allocCount = alignUpAllocCount(newCap + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);

        _impl.l.ptr      = static_cast<pointer>(realloc(_impl.l.ptr, allocSize));
        _impl.l.capacity = allocCount - 1;
    } else {
        if (newCap < SHORT_CAPACITY)
            return;

        const size_type originalSize = _impl.s.size;

        const size_type allocCount = alignUpAllocCount(newCap + 1);
        const size_type allocSize  = allocCount * sizeof(value_type);
        const pointer   buffer     = static_cast<pointer>(malloc(allocSize));

        // Copy with trailing '\0'.
        traits_type::copy(buffer, _impl.s.str, _impl.s.size + size_type(1));

        _impl.l.ptr      = buffer;
        _impl.l.capacity = allocCount - 1;
        _impl.l.size     = originalSize;
        _impl.l.padding  = 0;
        _impl.l.isLong   = 1;
    }
}

#if !defined(__clang__) && defined(_MSC_VER)
#    pragma warning(pop)
#endif

auto YaGE::String::Clear() noexcept -> void {
    if (_impl.l.isLong) {
        traits_type::assign(_impl.l.ptr[0], value_type());
        _impl.l.size = 0;
    } else {
        traits_type::assign(_impl.s.str[0], value_type());
        _impl.s.size = 0;
    }
}

auto YaGE::String::Insert(size_type position, StringView str) noexcept -> String & {
    const size_type originSize = Length();
    Reserve(originSize + str.Length());

    // Clamp position.
    position = (position > originSize ? originSize : position);

    const pointer buffer    = Data();
    const pointer bufferEnd = buffer + originSize;

    // Find insert position.
    const pointer insertTarget = buffer + position;

    // Move original data with trailing '\0'.
    const size_type moveCount = static_cast<size_type>(bufferEnd - insertTarget) + 1;
    traits_type::move(insertTarget + str.Length(), insertTarget, moveCount);

    // Insert data.
    traits_type::copy(insertTarget, str.Data(), str.Length());

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = originSize + str.Length();
    else
        _impl.s.size = static_cast<uint8_t>(originSize + str.Length());

    return *this;
}

auto YaGE::String::Insert(size_type position, size_type count, value_type ch) noexcept -> String & {
    const size_type originalSize = this->Length();
    Reserve(originalSize + count);

    // Clamp position.
    position = (position > originalSize ? originalSize : position);

    const pointer buffer    = Data();
    const pointer bufferEnd = buffer + originalSize;

    // Find insert position.
    const pointer insertTarget = buffer + position;

    // Move original data with trailing '\0'.
    const size_type moveCount = static_cast<size_type>(bufferEnd - insertTarget) + 1;
    traits_type::move(insertTarget + count, insertTarget, moveCount);

    // Insert data.
    traits_type::assign(insertTarget, count, ch);

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = originalSize + count;
    else
        _impl.s.size = static_cast<uint8_t>(originalSize + count);

    return *this;
}

auto YaGE::String::Remove(size_type from, size_type count) noexcept -> String & {
    const size_type length = this->Length();
    if (from >= length)
        return *this;

    const size_type maxRemoveCount = length - from;
    const size_type removeCount    = (count > maxRemoveCount ? maxRemoveCount : count);

    const pointer buffer      = Data();
    const pointer removeStart = buffer + from;
    const pointer removeEnd   = removeStart + removeCount;

    // Move characters with trailing '\0'.
    const size_type moveCount = length - removeCount - from + 1;
    traits_type::move(removeStart, removeEnd, moveCount);

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = length - removeCount;
    else
        _impl.s.size = static_cast<uint8_t>(length - removeCount);

    return *this;
}

auto YaGE::String::Remove(const_iterator first, const_iterator last) noexcept -> String & {
    if (first > last)
        std::swap(first, last);

    const pointer   buffer    = Data();
    const size_type length    = Length();
    const pointer   bufferEnd = buffer + length;

    // Move characters with trailing '\0'.
    const size_type moveCount = static_cast<size_type>(bufferEnd - last) + 1;
    traits_type::move(const_cast<pointer>(first), last, moveCount);

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = length - static_cast<size_type>(last - first);
    else
        _impl.s.size = static_cast<uint8_t>(length - static_cast<size_type>(last - first));

    return *this;
}

auto YaGE::String::RemoveAfter(size_type index) noexcept -> String & {
    const size_type length = Length();
    if (index >= length)
        return *this;

    const pointer buffer = Data();
    traits_type::assign(*(buffer + index), value_type());

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = index;
    else
        _impl.s.size = static_cast<uint8_t>(index);

    return *this;
}

auto YaGE::String::push_back(value_type ch) noexcept -> void {
    const size_type length = Length();
    const size_type cap    = Capacity();

    if (length == cap)
        Reserve(cap * 2);

    // Append data.
    const pointer buffer = Data();
    traits_type::assign(*(buffer + length), ch);
    traits_type::assign(*(buffer + length + 1), value_type());

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = length + 1;
    else
        _impl.s.size = static_cast<uint8_t>(length + 1);
}

auto YaGE::String::pop_back() noexcept -> void {
    const size_type length = Length();
    if (length == 0)
        return;

    const pointer buffer = Data();
    traits_type::assign(*(buffer + length - 1), value_type());

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = length - 1;
    else
        _impl.s.size = static_cast<uint8_t>(length - 1);
}

auto YaGE::String::Append(size_type count, value_type ch) noexcept -> String & {
    const size_type length = this->Length();
    const size_type cap    = Capacity();
    if (cap < count + length) {
        size_type reserveSize = length > count ? cap * 2 : length + count;
        Reserve(reserveSize);
    }

    // Append data.
    const pointer buffer = Data();
    traits_type::assign(buffer + length, count, ch);

    const size_type newSize = length + count;
    traits_type::assign(*(buffer + newSize), value_type());

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = newSize;
    else
        _impl.s.size = static_cast<uint8_t>(newSize);

    return *this;
}

auto YaGE::String::Append(const value_type *str, size_type count) noexcept -> String & {
    const size_type length = this->Length();
    const size_type cap    = Capacity();
    if (cap < count + length) {
        size_type reserveSize = length > count ? cap * 2 : length + count;
        Reserve(reserveSize);
    }

    // Append data.
    const pointer buffer = Data();
    traits_type::copy(buffer + length, str, count);

    const size_type newSize = length + count;
    traits_type::assign(*(buffer + newSize), value_type());

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = newSize;
    else
        _impl.s.size = static_cast<uint8_t>(newSize);

    return *this;
}

auto YaGE::String::Resize(size_type count, value_type ch) noexcept -> void {
    Reserve(count);

    const size_type length = this->Length();
    const pointer   buffer = Data();

    if (count > length)
        traits_type::assign(buffer + length, count - length, ch);
    traits_type::assign(*(buffer + count), value_type());

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = count;
    else
        _impl.s.size = static_cast<uint8_t>(count);
}

auto YaGE::String::TrimStart(StringView charSet) noexcept -> String & {
    const pointer   buffer    = Data();
    const size_type length    = Length();
    const pointer   bufferEnd = buffer + length;

    pointer trimEnd = buffer;
    while (trimEnd != bufferEnd && charSet.Contains(*trimEnd))
        ++trimEnd;

    // Move data with trailing '\0'.
    const size_type newSize = static_cast<size_type>(bufferEnd - trimEnd);
    traits_type::move(buffer, trimEnd, newSize + 1);

    // Update size.
    if (_impl.l.isLong)
        _impl.l.size = newSize;
    else
        _impl.s.size = static_cast<uint8_t>(newSize);

    return *this;
}

auto YaGE::String::TrimEnd(StringView charSet) noexcept -> String & {
    const pointer   buffer    = Data();
    const size_type length    = Length();
    const pointer   bufferEnd = buffer + length;

    pointer trimStart = bufferEnd;
    while (trimStart != buffer && charSet.Contains(*(trimStart - 1)))
        --trimStart;

    traits_type::assign(*trimStart, value_type());

    // Update size.
    const size_type newSize = static_cast<size_type>(trimStart - buffer);
    if (_impl.l.isLong)
        _impl.l.size = newSize;
    else
        _impl.s.size = static_cast<uint8_t>(newSize);

    return *this;
}

auto YaGE::String::ToLower() noexcept -> String & {
    const pointer   buffer = Data();
    const size_type length = Length();
    const pointer   strEnd = buffer + length;

    for (pointer i = buffer; i != strEnd; ++i) {
        if (*i >= value_type('A') && *i <= value_type('Z'))
            traits_type::assign(*i, *i - value_type('A') + value_type('a'));
    }

    return *this;
}

auto YaGE::String::ToUpper() noexcept -> String & {
    const pointer   buffer = Data();
    const size_type length = Length();
    const pointer   strEnd = buffer + length;

    for (pointer i = buffer; i != strEnd; ++i) {
        if (*i >= value_type('a') && *i <= value_type('z'))
            traits_type::assign(*i, *i - value_type('a') + value_type('A'));
    }

    return *this;
}

auto YaGE::operator+(const String &lhs, const String &rhs) noexcept -> String {
    String result;
    result.Reserve(lhs.Length() + rhs.Length());
    result.Append(lhs).Append(rhs);
    return result;
}

auto YaGE::operator+(const String &lhs, StringView rhs) noexcept -> String {
    String result;
    result.Reserve(lhs.Length() + rhs.Length());
    result.Append(lhs).Append(rhs);
    return result;
}

auto YaGE::operator+(StringView lhs, const String &rhs) noexcept -> String {
    String result;
    result.Reserve(lhs.Length() + rhs.Length());
    result.Append(lhs).Append(rhs);
    return result;
}

auto YaGE::operator+(const String &lhs, const char16_t *rhs) noexcept -> String {
    String result;
    result.Reserve(lhs.Length() + StringView(rhs).Length());
    result.Append(lhs).Append(rhs);
    return result;
}

auto YaGE::operator+(const char16_t *lhs, const String &rhs) noexcept -> String {
    String result;
    result.Reserve(StringView(lhs).Length() + rhs.Length());
    result.Append(lhs).Append(rhs);
    return result;
}
