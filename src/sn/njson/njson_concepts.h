#pragma once

#include <concepts> // For std::same_as.

#include "sn/njson/detail/njson_builtins.h"

namespace sn::detail::poison {

struct njson_overload_not_found {};

template<class T, class... Tags>
njson_overload_not_found try_to_njson(const T &src, nlohmann::json *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
njson_overload_not_found to_njson(const T &src, nlohmann::json *dst, Tags...) = delete;
template<class T, class... Tags>
njson_overload_not_found try_from_njson(const nlohmann::json &src, T *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
njson_overload_not_found from_njson(const nlohmann::json &src, T *dst, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept try_to_njsonable =
    requires(const T &src, nlohmann::json *dst, Tags... tags) { {try_to_njson(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept to_njsonable =
    requires(const T &src, nlohmann::json *dst, Tags... tags) { {to_njson(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

template<class T, class... Tags>
concept try_from_njsonable =
    requires(const nlohmann::json &src, T *dst, Tags... tags) { {try_from_njson(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_njsonable =
    requires(const nlohmann::json &src, T *dst, Tags... tags) { {from_njson(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::detail {

//
// Same as the concepts above, but variable templates can be declared before they're defined, and concepts can't. So
// builtins for templates like std::vector<T> can forward-declare these to check that T is supported.
//

template<class T, class... Tags>
constexpr bool is_try_to_njsonable_v = sn::detail::concepts::try_to_njsonable<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_to_njsonable_v = sn::detail::concepts::to_njsonable<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_try_from_njsonable_v = sn::detail::concepts::try_from_njsonable<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_from_njsonable_v = sn::detail::concepts::from_njsonable<T, Tags...>;

} // namespace sn::detail

namespace sn::concepts {

using sn::detail::concepts::try_to_njsonable;
using sn::detail::concepts::to_njsonable;
using sn::detail::concepts::try_from_njsonable;
using sn::detail::concepts::from_njsonable;

} // namespace sn::concepts
