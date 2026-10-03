#pragma once
@INCLUDES@
#include "sn/core/tag.h"
#include "sn/@LOWER@/@LOWER@_concepts.h"

#include "@LOWER@_builtins.h"

namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_to_@LOWER@() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_to_@LOWER@able<T>,
                      "Type T is not supported, did you forget to declare `bool try_to_@LOWER@(const T &src, @DST@)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_to_@LOWER@able<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_to_@LOWER@(const T &src, @DST@, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_to_@LOWER@() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::to_@LOWER@able<T>,
                      "Type T is not supported, did you forget to declare `void to_@LOWER@(const T &src, @DST@)` in T's namespace?");
    } else {
        static_assert(sn::concepts::to_@LOWER@able<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void to_@LOWER@(const T &src, @DST@, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_try_from_@LOWER@() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::try_from_@LOWER@able<T>,
                      "Type T is not supported, did you forget to declare `bool try_from_@LOWER@(@SRC@, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::try_from_@LOWER@able<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool try_from_@LOWER@(@SRC@, T *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_from_@LOWER@() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::from_@LOWER@able<T>,
                      "Type T is not supported, did you forget to declare `void from_@LOWER@(@SRC@, T *dst)` in T's namespace?");
    } else {
        static_assert(sn::concepts::from_@LOWER@able<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `void from_@LOWER@(@SRC@, T *dst, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_to_@LOWER@(const T &src, @DST@, Tags... tags) noexcept {
    validate_try_to_@LOWER@<T, Tags...>();
    using sn::detail::builtins::try_to_@LOWER@;
    return try_to_@LOWER@(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_to_@LOWER@(const T &src, @DST@, Tags... tags) {
    validate_to_@LOWER@<T, Tags...>();
    using sn::detail::builtins::to_@LOWER@;
    to_@LOWER@(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_from_@LOWER@(@SRC@, T *dst, Tags... tags) noexcept {
    validate_try_from_@LOWER@<T, Tags...>();
    using sn::detail::builtins::try_from_@LOWER@;
    return try_from_@LOWER@(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_from_@LOWER@(@SRC@, T *dst, Tags... tags) {
    validate_from_@LOWER@<T, Tags...>();
    using sn::detail::builtins::from_@LOWER@;
    from_@LOWER@(src, dst, tags...);
}

} // namespace sn::detail
