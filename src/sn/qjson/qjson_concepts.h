#pragma once

#include <concepts> // For std::same_as.

#include "sn/qstring/detail/qstring_builtins.h"

namespace sn::detail::poison {

struct qjson_overload_not_found {};

template<class T, class... Tags>
qjson_overload_not_found try_to_qjson(const T &, QJsonValue *, Tags...) noexcept = delete;
template<class T, class... Tags>
qjson_overload_not_found to_qjson(const T &src, QJsonValue *, Tags...) = delete;
template<class T, class... Tags>
qjson_overload_not_found try_from_qjson(const QJsonValue &src, T *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
qjson_overload_not_found from_qjson(const QJsonValue &src, T *dst, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept try_to_qjsonable =
    requires(const T &src, QJsonValue *dst, Tags... tags) { {try_to_qjson(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept to_qjsonable =
    requires(const T &src, QJsonValue *dst, Tags... tags) { {to_qjson(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

template<class T, class... Tags>
concept try_from_qjsonable =
    requires(const QJsonValue &src, T *dst, Tags... tags) { {try_from_qjson(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_qjsonable =
    requires(const QJsonValue &src, T *dst, Tags... tags) { {from_qjson(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::concepts {

using sn::detail::concepts::try_to_qjsonable;
using sn::detail::concepts::to_qjsonable;
using sn::detail::concepts::try_from_qjsonable;
using sn::detail::concepts::from_qjsonable;

} // namespace sn::concepts
