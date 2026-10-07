#pragma once

#include <type_traits>

namespace sn::detail {

/**
 * Underlying type for enums, and the type itself for everything else.
 *
 * @tparam T                            Type to check.
 */
template<class T>
using underlying_type_ex_t = typename std::conditional_t<std::is_enum_v<T>, std::underlying_type<T>, std::type_identity<T>>::type;

/**
 * Same as `std::is_signed_v`, but also works for enums.
 *
 * @tparam T                            Type to check.
 */
template<class T>
constexpr bool is_signed_ex_v = std::is_signed_v<underlying_type_ex_t<T>>;

} // namespace sn::detail
