#include "StringView.h"

using namespace YaGE;

YAGE_NODISCARD auto YaGE::StringView::Split(StringView delims) const noexcept -> std::vector<StringView> {
    std::vector<StringView> result;

    const_pointer       lastSplit = ptr;
    const const_pointer cmpEnd    = ptr + len;

    for (const_pointer i = ptr; i != cmpEnd; ++i) {
        if (delims.Contains(*i)) {
            if (i != lastSplit)
                result.emplace_back(lastSplit, static_cast<size_type>(i - lastSplit));
            lastSplit = i + 1;
        }
    }

    if (lastSplit < ptr + len)
        result.emplace_back(lastSplit, static_cast<size_type>(cmpEnd - lastSplit));

    return result;
}

YAGE_NODISCARD auto YaGE::StringView::Split(value_type delim) const noexcept -> std::vector<StringView> {
    std::vector<StringView> result;

    const_pointer       lastSplit = ptr;
    const const_pointer cmpEnd    = ptr + len;

    for (const_pointer i = ptr; i != cmpEnd; ++i) {
        if (traits_type::eq(*i, delim)) {
            if (i != lastSplit)
                result.emplace_back(lastSplit, static_cast<size_type>(i - lastSplit));
            lastSplit = i + 1;
        }
    }

    if (lastSplit < ptr + len)
        result.emplace_back(lastSplit, static_cast<size_type>(cmpEnd - lastSplit));

    return result;
}
