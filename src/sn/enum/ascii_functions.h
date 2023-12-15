#pragma once

#include <string_view>

namespace sn::detail {

template<class Char>
[[nodiscard]] constexpr Char to_lower_ascii(Char c) noexcept {
    return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
}

template<class Char>
constexpr std::basic_string_view<Char> to_lower_ascii(std::basic_string_view<Char> src, Char *buffer) noexcept {
    const Char *src_ptr = src.data();
    const Char *src_end = src.data() + src.size();
    Char *dst_ptr = buffer;

    while (src_ptr < src_end)
        *dst_ptr++ = to_lower_ascii(*src_ptr++);

    return {buffer, src.size()};
}

} // namespace sn::detail
