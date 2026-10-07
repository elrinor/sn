#pragma once

#include <concepts> // For std::same_as.

#include "sn/@LOWER@/detail/@LOWER@_builtins.h"

namespace sn::detail::poison {

struct @LOWER@_overload_not_found {};

template<class T, class... Tags>
@LOWER@_overload_not_found try_to_@LOWER@(const T &src, @DST@, Tags...) noexcept = delete;
template<class T, class... Tags>
@LOWER@_overload_not_found to_@LOWER@(const T &src, @DST@, Tags...) = delete;
template<class T, class... Tags>
@LOWER@_overload_not_found try_from_@LOWER@(@SRC@, T *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
@LOWER@_overload_not_found from_@LOWER@(@SRC@, T *dst, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept try_to_@LOWER@able =
    requires(const T &src, @DST@, Tags... tags) { {try_to_@LOWER@(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept to_@LOWER@able =
    requires(const T &src, @DST@, Tags... tags) { {to_@LOWER@(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

template<class T, class... Tags>
concept try_from_@LOWER@able =
    requires(@SRC@, T *dst, Tags... tags) { {try_from_@LOWER@(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_@LOWER@able =
    requires(@SRC@, T *dst, Tags... tags) { {from_@LOWER@(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::detail {

//
// Same as the concepts above, but variable templates can be declared before they're defined, and concepts can't. So
// builtins for templates like std::vector<T> can forward-declare these to check that T is supported.
//

template<class T, class... Tags>
constexpr bool is_try_to_@LOWER@able_v = sn::detail::concepts::try_to_@LOWER@able<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_to_@LOWER@able_v = sn::detail::concepts::to_@LOWER@able<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_try_from_@LOWER@able_v = sn::detail::concepts::try_from_@LOWER@able<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_from_@LOWER@able_v = sn::detail::concepts::from_@LOWER@able<T, Tags...>;

} // namespace sn::detail

namespace sn::concepts {

using sn::detail::concepts::try_to_@LOWER@able;
using sn::detail::concepts::to_@LOWER@able;
using sn::detail::concepts::try_from_@LOWER@able;
using sn::detail::concepts::from_@LOWER@able;

} // namespace sn::concepts
