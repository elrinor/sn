#pragma once

#include <cstddef>
#include <string> // For _LIBCPP_VERSION.

namespace sn::detail {

/**
 * Capacity of the small string buffer, which is what `std::string().capacity()` returns. Pinned by a test in
 * `string_ut.cpp`.
 */
#if defined(_LIBCPP_VERSION)
inline constexpr std::size_t small_string_capacity = 3 * sizeof(std::size_t) - 2; // 22 chars on 64-bit platforms, 10 on 32-bit.
#else
inline constexpr std::size_t small_string_capacity = 15; // libstdc++ and msvc.
#endif

} // namespace sn::detail
