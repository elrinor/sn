#pragma once

#include <concepts> // For std::same_as.

#include "sn/qstring/detail/qstring_builtins.h"

namespace sn::detail::poison {

struct qstring_overload_not_found {};

template<class T, class... Tags>
qstring_overload_not_found try_to_qstring(const T &src, QString *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
qstring_overload_not_found to_qstring(const T &src, QString *dst, Tags...) = delete;
template<class T, class... Tags>
qstring_overload_not_found try_from_qstring(QStringView src, T *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
qstring_overload_not_found from_qstring(QStringView src, T *dst, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept try_to_qstringable =
    requires(const T &src, QString *dst, Tags... tags) { {try_to_qstring(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept to_qstringable =
    requires(const T &src, QString *dst, Tags... tags) { {to_qstring(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

template<class T, class... Tags>
concept try_from_qstringable =
    requires(QStringView src, T *dst, Tags... tags) { {try_from_qstring(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_qstringable =
    requires(QStringView src, T *dst, Tags... tags) { {from_qstring(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::concepts {

using sn::detail::concepts::try_to_qstringable;
using sn::detail::concepts::to_qstringable;
using sn::detail::concepts::try_from_qstringable;
using sn::detail::concepts::from_qstringable;

} // namespace sn::concepts
