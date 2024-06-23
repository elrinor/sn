#pragma once
@INCLUDES@
#include "sn/core/tag.h"
#include "sn/@LOWER@/detail/@LOWER@_dispatch.h"

namespace sn {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_to_@LOWER@(const T &src, @DST@, Tags... tags) noexcept {
    return sn::detail::do_try_to_@LOWER@(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void to_@LOWER@(const T &src, @DST@, Tags... tags) {
    sn::detail::do_to_@LOWER@(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] @TYPE@ to_@LOWER@(const T &src, Tags... tags) {
    @TYPE@ result;
    sn::detail::do_to_@LOWER@(src, &result, tags...);
    return result;
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_from_@LOWER@(@SRC@, T *dst, Tags... tags) noexcept {
    return sn::detail::do_try_from_@LOWER@(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void from_@LOWER@(@SRC@, T *dst, Tags... tags) {
    sn::detail::do_from_@LOWER@(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] T from_@LOWER@(@SRC@, Tags... tags) {
    T result;
    sn::detail::do_from_@LOWER@(src, &result, tags...);
    return result;
}

} // namespace sn
