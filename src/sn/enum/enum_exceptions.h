#pragma once

#include <string_view>

#include "sn/core/type_name.h"
#include "sn/string/string.h"
#include "sn/workaround/to_underlying.h"

namespace sn {

[[noreturn]] void throw_enum_to_string_error(std::string_view type_name, std::string_view value);
[[noreturn]] void throw_enum_from_string_error(std::string_view type_name, std::string_view value);

template<class T>
[[noreturn]] void throw_enum_to_string_error(const T &value) {
    throw_enum_to_string_error(sn::type_name<T>(), sn::to_string(sn::to_underlying(value)));
}

template<class T>
[[noreturn]] void throw_enum_from_string_error(std::string_view value) {
    throw_enum_from_string_error(sn::type_name<T>(), value);
}

} // namespace sn
