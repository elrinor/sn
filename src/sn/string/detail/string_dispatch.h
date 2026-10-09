#pragma once

#include <string>
#include <string_view>

#include "sn/core/error_fwd.h"
#include "sn/core/tag.h"
#include "sn/string/string_concepts.h"

#include "string_builtins.h"

namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
consteval void validate_to_string() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::to_stringable<T>,
                      "Type T is not supported, did you forget to declare `bool to_string(const T &src, std::string *dst, sn::error *err)` in T's namespace?");
    } else {
        static_assert(sn::concepts::to_stringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool to_string(const T &src, std::string *dst, sn::error *err, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_from_string() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::from_stringable<T>,
                      "Type T is not supported, did you forget to declare `bool from_string(std::string_view src, T *dst, sn::error *err)` in T's namespace?");
    } else {
        static_assert(sn::concepts::from_stringable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool from_string(std::string_view src, T *dst, sn::error *err, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_to_string(const T &src, std::string *dst, sn::error *err, Tags... tags) {
    validate_to_string<T, Tags...>();
    using sn::detail::builtins::to_string;
    return to_string(src, dst, err, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_from_string(std::string_view src, T *dst, sn::error *err, Tags... tags) {
    validate_from_string<T, Tags...>();
    using sn::detail::builtins::from_string;
    return from_string(src, dst, err, tags...);
}

} // namespace sn::detail
