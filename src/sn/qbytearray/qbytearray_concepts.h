#pragma once

#include <concepts> // For std::same_as.

#include "sn/qbytearray/detail/qbytearray_builtins.h"

namespace sn::detail::poison {

struct qbytearray_overload_not_found {};

template<class T, class... Tags>
qbytearray_overload_not_found try_to_qbytearray(const T &src, QByteArray *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
qbytearray_overload_not_found to_qbytearray(const T &src, QByteArray *dst, Tags...) = delete;
template<class T, class... Tags>
qbytearray_overload_not_found try_from_qbytearray(QByteArrayView src, T *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
qbytearray_overload_not_found from_qbytearray(QByteArrayView src, T *dst, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept try_to_qbytearrayable =
    requires(const T &src, QByteArray *dst, Tags... tags) { {try_to_qbytearray(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept to_qbytearrayable =
    requires(const T &src, QByteArray *dst, Tags... tags) { {to_qbytearray(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

template<class T, class... Tags>
concept try_from_qbytearrayable =
    requires(QByteArrayView src, T *dst, Tags... tags) { {try_from_qbytearray(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_qbytearrayable =
    requires(QByteArrayView src, T *dst, Tags... tags) { {from_qbytearray(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::concepts {

using sn::detail::concepts::try_to_qbytearrayable;
using sn::detail::concepts::to_qbytearrayable;
using sn::detail::concepts::try_from_qbytearrayable;
using sn::detail::concepts::from_qbytearrayable;

} // namespace sn::concepts
