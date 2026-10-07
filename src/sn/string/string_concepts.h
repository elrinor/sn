#pragma once

#include <concepts> // For std::same_as.

#include "sn/string/detail/string_builtins.h"

namespace sn::detail::poison {

struct string_overload_not_found {};

template<class T, class... Tags>
string_overload_not_found try_to_string(const T &src, std::string *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
string_overload_not_found to_string(const T &src, std::string *dst, Tags...) = delete;
template<class T, class... Tags>
string_overload_not_found try_from_string(std::string_view src, T *dst, Tags...) noexcept = delete;
template<class T, class... Tags>
string_overload_not_found from_string(std::string_view src, T *dst, Tags...) = delete;

} // namespace sn::detail::poison

namespace sn::detail::concepts {

using namespace sn::detail::builtins; // NOLINT
using namespace sn::detail::poison; // NOLINT

template<class T, class... Tags>
concept try_to_stringable =
    requires(const T &src, std::string *dst, Tags... tags) { {try_to_string(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept to_stringable =
    requires(const T &src, std::string *dst, Tags... tags) { {to_string(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

template<class T, class... Tags>
concept try_from_stringable =
    requires(std::string_view src, T *dst, Tags... tags) { {try_from_string(src, dst, tags...)} -> std::same_as<bool>; }; // NOLINT

template<class T, class... Tags>
concept from_stringable =
    requires(std::string_view src, T *dst, Tags... tags) { {from_string(src, dst, tags...)} -> std::same_as<void>; }; // NOLINT

} // namespace sn::detail::concepts

namespace sn::detail {

//
// Same as the concepts above, but variable templates can be declared before they're defined, and concepts can't. So
// builtins for templates like std::vector<T> can forward-declare these to check that T is supported.
//

template<class T, class... Tags>
constexpr bool is_try_to_stringable_v = sn::detail::concepts::try_to_stringable<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_to_stringable_v = sn::detail::concepts::to_stringable<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_try_from_stringable_v = sn::detail::concepts::try_from_stringable<T, Tags...>;

template<class T, class... Tags>
constexpr bool is_from_stringable_v = sn::detail::concepts::from_stringable<T, Tags...>;

} // namespace sn::detail

namespace sn::concepts {

using sn::detail::concepts::try_to_stringable;
using sn::detail::concepts::to_stringable;
using sn::detail::concepts::try_from_stringable;
using sn::detail::concepts::from_stringable;

} // namespace sn::concepts
