#pragma once

#include <concepts> // For std::same_as.

#include "sn/core/error_fwd.h"
#include "sn/string/detail/string_builtins.h"

namespace sn::detail::poison {

struct string_overload_not_found {};

template<class T, class... Tags>
string_overload_not_found to_string(const T &src, std::string *dst, sn::error *err, Tags...) = delete;
template<class T, class... Tags>
string_overload_not_found from_string(std::string_view src, T *dst, sn::error *err, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept to_stringable =
    requires(const T &src, std::string *dst, sn::error *err, Tags... tags) { {to_string(src, dst, err, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_stringable =
    requires(std::string_view src, T *dst, sn::error *err, Tags... tags) { {from_string(src, dst, err, tags...)} -> std::same_as<bool>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::concepts {

using sn::detail::concepts::to_stringable;
using sn::detail::concepts::from_stringable;

} // namespace sn::concepts
