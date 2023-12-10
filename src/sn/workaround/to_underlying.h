#pragma once

#include <type_traits>

namespace sn {

// TODO(elric): #cpp23 drop this.
template <class T>
[[nodiscard]] inline constexpr std::underlying_type_t<T> to_underlying(T value) noexcept {
    return static_cast<std::underlying_type_t<T>>(value);
}

} // namespace sn
