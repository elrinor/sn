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

namespace sn::concepts {

using sn::detail::concepts::try_to_@LOWER@able;
using sn::detail::concepts::to_@LOWER@able;
using sn::detail::concepts::try_from_@LOWER@able;
using sn::detail::concepts::from_@LOWER@able;

} // namespace sn::concepts
