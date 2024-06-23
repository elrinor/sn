#pragma once

#include <string>
#include <string_view>

#include "sn/core/tag.h"
#include "sn/string/string_concepts.h"

#include "string_builtins.h"

namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_to_string() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_to_stringable<T>,
                      "Type T is not supported, did you forget to declare `bool try_to_string(const T &src, std::string *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_to_stringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_to_string(const T &src, std::string *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_to_string() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::to_stringable<T>,
                      "Type T is not supported, did you forget to declare `void to_string(const T &src, std::string *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::to_stringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void to_string(const T &src, std::string *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_from_string() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_from_stringable<T>,
                      "Type T is not supported, did you forget to declare `bool try_from_string(std::string_view src, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_from_stringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_from_string(std::string_view src, T *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_from_string() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::from_stringable<T>,
                      "Type T is not supported, did you forget to declare `void from_string(std::string_view src, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::from_stringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void from_string(std::string_view src, T *dst, Tags...)` in T's namespace?");
    }
}

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
