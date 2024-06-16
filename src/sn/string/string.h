#pragma once

#include <string>
#include <string_view>

#include "sn/core/tag.h"
#include "sn/string/detail/string_builtins.h"
#include "sn/detail/codegen/validation.h"

#include "string_concepts.h"

namespace sn::detail {

_SN_DEFINE_VALIDATION_FUNCTIONS(string, std::string *dst, std::string_view src)

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_to_string(const T &src, std::string *dst, Tags... tags) noexcept {
    validate_try_to_string<T, Tags...>();
    using sn::detail::builtins::try_to_string;
    return try_to_string(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_to_string(const T &src, std::string *dst, Tags... tags) {
    validate_to_string<T, Tags...>();
    using sn::detail::builtins::to_string;
    to_string(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_from_string(std::string_view src, T *dst, Tags... tags) noexcept {
    validate_try_from_string<T, Tags...>();
    using sn::detail::builtins::try_from_string;
    return try_from_string(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_from_string(std::string_view src, T *dst, Tags... tags) {
    validate_from_string<T, Tags...>();
    using sn::detail::builtins::from_string;
    from_string(src, dst, tags...);
}

} // namespace sn::detail

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
