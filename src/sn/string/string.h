#pragma once

#include <string>
#include <string_view>

#include "sn/core/tag.h"
#include "sn/string/detail/string_dispatch.h"

namespace sn {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_to_string(const T &src, std::string *dst, Tags... tags) noexcept {
    return sn::detail::do_try_to_string(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void to_string(const T &src, std::string *dst, Tags... tags) {
    sn::detail::do_to_string(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] std::string to_string(const T &src, Tags... tags) {
    std::string result;
    sn::detail::do_to_string(src, &result, tags...);
    return result;
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_from_string(std::string_view src, T *dst, Tags... tags) noexcept {
    return sn::detail::do_try_from_string(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void from_string(std::string_view src, T *dst, Tags... tags) {
    sn::detail::do_from_string(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] T from_string(std::string_view src, Tags... tags) {
    T result;
    sn::detail::do_from_string(src, &result, tags...);
    return result;
}

} // namespace sn
