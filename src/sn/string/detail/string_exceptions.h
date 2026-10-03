#pragma once

#include <system_error>
#include <string_view>

#include "sn/core/type_name.h"

namespace sn::detail {

[[noreturn]] void throw_to_string_error(std::string_view type_name);
[[noreturn]] void throw_from_string_error(std::string_view type_name, std::string_view value);
[[noreturn]] void throw_number_from_string_error(std::string_view type_name, std::string_view value, std::errc error);

template<class T>
[[noreturn]] void throw_to_string_error() {
    throw_to_string_error(sn::type_name<T>());
}

template<class T>
[[noreturn]] void throw_from_string_error(std::string_view value) {
    throw_from_string_error(sn::type_name<T>(), value);
}

template<class T>
[[noreturn]] void throw_number_from_string_error(std::string_view value, std::errc error) {
    throw_number_from_string_error(sn::type_name<T>(), value, error);
}

} // namespace sn::detail
