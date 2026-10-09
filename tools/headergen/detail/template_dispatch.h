#pragma once
@INCLUDES@
#include "sn/core/error_fwd.h"
#include "sn/core/tag.h"
#include "sn/@LOWER@/@LOWER@_concepts.h"

#include "@LOWER@_builtins.h"

namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
consteval void validate_to_@LOWER@() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::to_@LOWER@able<T>,
                      "Type T is not supported, did you forget to declare `bool to_@LOWER@(const T &src, @DST@, sn::error *err)` in T's namespace?");
    } else {
        static_assert(sn::concepts::to_@LOWER@able<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool to_@LOWER@(const T &src, @DST@, sn::error *err, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
consteval void validate_from_@LOWER@() {
    if constexpr (sizeof...(Tags) == 0) {
        static_assert(sn::concepts::from_@LOWER@able<T>,
                      "Type T is not supported, did you forget to declare `bool from_@LOWER@(@SRC@, T *dst, sn::error *err)` in T's namespace?");
    } else {
        static_assert(sn::concepts::from_@LOWER@able<T, Tags...>,
                      "Type T with provided Tags is not supported, did you forget to declare `bool from_@LOWER@(@SRC@, T *dst, sn::error *err, Tags...)` in T's namespace?");
    }
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_to_@LOWER@(const T &src, @DST@, sn::error *err, Tags... tags) {
    validate_to_@LOWER@<T, Tags...>();
    using sn::detail::builtins::to_@LOWER@;
    return to_@LOWER@(src, dst, err, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_from_@LOWER@(@SRC@, T *dst, sn::error *err, Tags... tags) {
    validate_from_@LOWER@<T, Tags...>();
    using sn::detail::builtins::from_@LOWER@;
    return from_@LOWER@(src, dst, err, tags...);
}

} // namespace sn::detail
