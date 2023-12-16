#pragma once

#include <string_view>
#include <type_traits>

#include "sn/workaround/to_underlying.h"
#include "sn/string/string.h"

namespace sn {

[[noreturn]] void throw_enum_to_string_error(std::string_view type_name, std::string_view value);
[[noreturn]] void throw_enum_from_string_error(std::string_view type_name, std::string_view value);

// TODO(elric): drop this.
template<class T> requires std::is_enum_v<T> || std::is_integral_v<T>
[[noreturn]] void throw_enum_to_string_error(std::string_view type_name, T value) {
    if constexpr (std::is_enum_v<T>) {
        throw_enum_to_string_error(type_name, sn::detail::std_to_underlying(value));
    } else if constexpr (std::is_signed_v<T>) {
        throw_enum_to_string_error(type_name, sn::to_string(static_cast<std::int64_t>(value)));
    } else {
        throw_enum_to_string_error(type_name, sn::to_string(static_cast<std::uint64_t>(value)));
    }
}

} // namespace sn
