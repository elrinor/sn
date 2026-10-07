#pragma once

#include <nlohmann/json_fwd.hpp>

#include "sn/core/tag.h"
#include "sn/njson/njson_concepts.h"

#include "njson_builtins.h"

namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_to_njson() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_to_njsonable<T>,
                      "Type T is not supported, did you forget to declare `bool try_to_njson(const T &src, nlohmann::json *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_to_njsonable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_to_njson(const T &src, nlohmann::json *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_to_njson() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::to_njsonable<T>,
                      "Type T is not supported, did you forget to declare `void to_njson(const T &src, nlohmann::json *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::to_njsonable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void to_njson(const T &src, nlohmann::json *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_from_njson() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_from_njsonable<T>,
                      "Type T is not supported, did you forget to declare `bool try_from_njson(const nlohmann::json &src, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_from_njsonable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_from_njson(const nlohmann::json &src, T *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_from_njson() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::from_njsonable<T>,
                      "Type T is not supported, did you forget to declare `void from_njson(const nlohmann::json &src, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::from_njsonable<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void from_njson(const nlohmann::json &src, T *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_to_njson(const T &src, nlohmann::json *dst, Tags... tags) noexcept {
    validate_try_to_njson<T, Tags...>();
    using sn::detail::builtins::try_to_njson;
    return try_to_njson(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_to_njson(const T &src, nlohmann::json *dst, Tags... tags) {
    validate_to_njson<T, Tags...>();
    using sn::detail::builtins::to_njson;
    to_njson(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_from_njson(const nlohmann::json &src, T *dst, Tags... tags) noexcept {
    validate_try_from_njson<T, Tags...>();
    using sn::detail::builtins::try_from_njson;
    return try_from_njson(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_from_njson(const nlohmann::json &src, T *dst, Tags... tags) {
    validate_from_njson<T, Tags...>();
    using sn::detail::builtins::from_njson;
    from_njson(src, dst, tags...);
}

} // namespace sn::detail
