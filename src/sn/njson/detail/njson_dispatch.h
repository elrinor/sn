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

/**
 * @internal
 *
 * Gives builtins access to the concepts and functions above.
 *
 * Builtins are declared before the concepts, so they can't name them, and they can't call `sn::to_njson` either.
 * Builtins for templates like `std::vector<T>` need both - to check that `T` is supported, and to convert the
 * elements. Names used inside a class template are looked up when it's instantiated, and by then this whole header is
 * visible. So such builtins go through this class instead.
 */
template<class T, class... Tags>
struct njson_dispatcher {
    [[nodiscard]] static consteval bool is_try_to_njsonable() {
        return sn::concepts::try_to_njsonable<T, Tags...>;
    }

    [[nodiscard]] static consteval bool is_to_njsonable() {
        return sn::concepts::to_njsonable<T, Tags...>;
    }

    [[nodiscard]] static consteval bool is_try_from_njsonable() {
        return sn::concepts::try_from_njsonable<T, Tags...>;
    }

    [[nodiscard]] static consteval bool is_from_njsonable() {
        return sn::concepts::from_njsonable<T, Tags...>;
    }

    [[nodiscard]] static bool try_to_njson(const T &src, nlohmann::json *dst, Tags... tags) noexcept {
        return do_try_to_njson(src, dst, tags...);
    }

    static void to_njson(const T &src, nlohmann::json *dst, Tags... tags) {
        do_to_njson(src, dst, tags...);
    }

    [[nodiscard]] static bool try_from_njson(const nlohmann::json &src, T *dst, Tags... tags) noexcept {
        return do_try_from_njson(src, dst, tags...);
    }

    static void from_njson(const nlohmann::json &src, T *dst, Tags... tags) {
        do_from_njson(src, dst, tags...);
    }
};

} // namespace sn::detail
