#pragma once

#include <type_traits>

namespace sn::detail {

// TODO(elric): #cpp23 drop this.
template<class T>
[[nodiscard]] constexpr std::underlying_type_t<T> std_to_underlying(T value) noexcept {
    return static_cast<std::underlying_type_t<T>>(value);
}

} // namespace sn::detail
