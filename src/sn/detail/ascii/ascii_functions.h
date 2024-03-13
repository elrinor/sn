#pragma once

#include <string_view>

namespace sn::detail {

template<class Char>
[[nodiscard]] constexpr Char to_lower_ascii(Char c) noexcept {
    return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
}

constexpr std::string_view to_lower_ascii(std::string_view src, char *buffer) noexcept {
    const char *src_ptr = src.data();
    const char *src_end = src.data() + src.size();
    char *dst_ptr = buffer;

    while (src_ptr < src_end)
        *dst_ptr++ = to_lower_ascii(*src_ptr++);

    return {buffer, src.size()};
}

} // namespace sn::detail
