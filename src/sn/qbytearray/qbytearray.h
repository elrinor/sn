#pragma once

#include "sn/core/tag.h"
#include "sn/qbytearray/detail/qbytearray_builtins.h"
#include "sn/detail/codegen/validation.h"

#include "qbytearray_concepts.h"

namespace sn::detail {

_SN_DEFINE_VALIDATION_FUNCTIONS(qbytearray, QByteArray *dst, QByteArrayView src)

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_to_qbytearray(const T &src, QByteArray *dst, Tags... tags) noexcept {
    validate_try_to_qbytearray<T, Tags...>();
    using sn::detail::builtins::try_to_qbytearray;
    return try_to_qbytearray(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_to_qbytearray(const T &src, QByteArray *dst, Tags... tags) {
    validate_to_qbytearray<T, Tags...>();
    using sn::detail::builtins::to_qbytearray;
    to_qbytearray(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool do_try_from_qbytearray(QByteArrayView src, T *dst, Tags... tags) noexcept {
    validate_try_from_qbytearray<T, Tags...>();
    using sn::detail::builtins::try_from_qbytearray;
    return try_from_qbytearray(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void do_from_qbytearray(QByteArrayView src, T *dst, Tags... tags) {
    validate_from_qbytearray<T, Tags...>();
    using sn::detail::builtins::from_qbytearray;
    from_qbytearray(src, dst, tags...);
}

} // namespace sn::detail

namespace sn {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_to_qbytearray(const T &src, QByteArray *dst, Tags... tags) noexcept {
    return sn::detail::do_try_to_qbytearray(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void to_qbytearray(const T &src, QByteArray *dst, Tags... tags) {
    sn::detail::do_to_qbytearray(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] QByteArray to_qbytearray(const T &src, Tags... tags) {
    QByteArray result;
    sn::detail::do_to_qbytearray(src, &result, tags...);
    return result;
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] bool try_from_qbytearray(QByteArrayView src, T *dst, Tags... tags) noexcept {
    return sn::detail::do_try_from_qbytearray(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
void from_qbytearray(QByteArrayView src, T *dst, Tags... tags) {
    sn::detail::do_from_qbytearray(src, dst, tags...);
}

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] T from_qbytearray(QByteArrayView src, Tags... tags) {
    T result;
    sn::detail::do_from_qbytearray(src, &result, tags...);
    return result;
}

} // namespace sn
