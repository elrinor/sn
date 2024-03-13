#pragma once

#include <string>
#include <string_view>

#include "sn/core/tags.h"
#include "sn/string/detail/string_builtins.h"

#include "string_concepts.h"

namespace sn::detail {

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool do_try_to_string(const T &src, std::string *dst, Tags... tags) noexcept {
    static_assert(stringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_STRING_FUNCTIONS?");
    using sn::builtins::try_to_string;
    return try_to_string(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void do_to_string(const T &src, std::string *dst, Tags... tags) {
    static_assert(stringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_STRING_FUNCTIONS?");
    using sn::builtins::to_string;
    to_string(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool do_try_from_string(std::string_view src, T *dst, Tags... tags) noexcept {
    static_assert(stringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_STRING_FUNCTIONS?");
    using sn::builtins::try_from_string;
    return try_from_string(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void do_from_string(std::string_view src, T *dst, Tags... tags) {
    static_assert(stringable<T, Tags...>, "Type T is not supported, did you forget to invoke SN_DECLARE_STRING_FUNCTIONS?");
    using sn::builtins::from_string;
    from_string(src, dst, tags...);
}

} // namespace sn::detail

namespace sn::niebloids {

struct to_string {
    template<class T, sn::any_tag... Tags>
    [[nodiscard]] std::string operator()(const T &src, Tags... tags) const {
        std::string result;
        sn::detail::do_to_string(src, &result, tags...);
        return result;
    }
};

template<class T>
struct from_string {
    template<sn::any_tag... Tags>
    [[nodiscard]] T operator()(std::string_view src, Tags... tags) const {
        T result;
        sn::detail::do_from_string(src, &result, tags...);
        return result;
    }
};

} // namespace sn::niebloids

namespace sn {

inline constexpr sn::niebloids::to_string to_string_v;
template<class T>
inline constexpr sn::niebloids::from_string<T> from_string_v;

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool try_to_string(const T &src, std::string *dst, Tags... tags) noexcept {
    return sn::detail::do_try_to_string(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void to_string(const T &src, std::string *dst, Tags... tags) {
    sn::detail::do_to_string(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] std::string to_string(const T &src, Tags... tags) {
    std::string result;
    sn::detail::do_to_string(src, &result, tags...);
    return result;
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] bool try_from_string(std::string_view src, T *dst, Tags... tags) noexcept {
    return sn::detail::do_try_from_string(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
void from_string(std::string_view src, T *dst, Tags... tags) {
    sn::detail::do_from_string(src, dst, tags...);
}

template<class T, sn::any_tag... Tags>
[[nodiscard]] T from_string(std::string_view src, Tags... tags) {
    T result;
    sn::detail::do_from_string(src, &result, tags...);
    return result;
}

} // namespace sn
