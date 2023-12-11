#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "sn/core/type_name.h"
#include "sn/string/string.h"
#include "sn/workaround/to_underlying.h"

namespace sn {
namespace detail {

template<class T> requires std::is_integral_v<T>
constexpr auto to_integer_representation(T value) {
    static_assert(sizeof(T) <= 8); // Should fit into int64.
    return static_cast<std::conditional_t<std::is_signed_v<T>, std::int64_t, std::uint64_t>>(value);
}

template<class T> requires std::is_enum_v<T>
constexpr auto to_integer_representation(T value) {
    return to_integer_representation(sn::detail::std_to_underlying(value));
}

} // namespace detail

[[noreturn]] void throw_enum_to_string_error(std::string_view type_name, std::string_view value);
[[noreturn]] void throw_enum_from_string_error(std::string_view type_name, std::string_view value);

template<class T>
[[noreturn]] void throw_enum_to_string_error(const T &value) {
    throw_enum_to_string_error(sn::type_name<T>(), sn::to_string(sn::detail::to_integer_representation(value)));
}

template<class T>
[[noreturn]] void throw_enum_from_string_error(std::string_view value) {
    throw_enum_from_string_error(sn::type_name<T>(), value);
}

} // namespace sn
