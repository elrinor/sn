#pragma once

#include <concepts> // For std::same_as.

#include "sn/core/error_fwd.h"
#include "sn/@LOWER@/detail/@LOWER@_builtins.h"

namespace sn::detail::poison {

struct @LOWER@_overload_not_found {};

template<class T, class... Tags>
@LOWER@_overload_not_found to_@LOWER@(const T &src, @DST@, sn::error *err, Tags...) = delete;
template<class T, class... Tags>
@LOWER@_overload_not_found from_@LOWER@(@SRC@, T *dst, sn::error *err, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept to_@LOWER@able =
    requires(const T &src, @DST@, sn::error *err, Tags... tags) { {to_@LOWER@(src, dst, err, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_@LOWER@able =
    requires(@SRC@, T *dst, sn::error *err, Tags... tags) { {from_@LOWER@(src, dst, err, tags...)} -> std::same_as<bool>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::concepts {

using sn::detail::concepts::to_@LOWER@able;
using sn::detail::concepts::from_@LOWER@able;

} // namespace sn::concepts
