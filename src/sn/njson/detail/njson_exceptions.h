#pragma once

#include <cstddef> // For std::size_t.
#include <string_view>

#include <nlohmann/json_fwd.hpp>

#include "sn/core/type_name.h"

namespace sn::detail {

[[noreturn]] void throw_to_njson_error(std::string_view type_name, std::string_view value);
[[noreturn]] void throw_from_njson_error(std::string_view type_name, const nlohmann::json &value);

//
// Functions below are for errors in nested values, `reason` is the error message for the nested value.
//

[[noreturn]] void throw_element_to_njson_error(std::size_t index, std::string_view reason);
[[noreturn]] void throw_element_from_njson_error(std::size_t index, std::string_view reason);
[[noreturn]] void throw_member_to_njson_error(std::string_view key, std::string_view reason);
[[noreturn]] void throw_member_from_njson_error(std::string_view key, std::string_view reason);
[[noreturn]] void throw_field_to_njson_error(std::string_view type_name, std::string_view field_name, std::string_view reason);
[[noreturn]] void throw_field_from_njson_error(std::string_view type_name, std::string_view field_name, std::string_view reason);
[[noreturn]] void throw_missing_field_from_njson_error(std::string_view type_name, std::string_view field_name);

template<class T>
[[noreturn]] void throw_to_njson_error(std::string_view value) {
    throw_to_njson_error(sn::type_name<T>(), value);
}

template<class T>
[[noreturn]] void throw_from_njson_error(const nlohmann::json &value) {
    throw_from_njson_error(sn::type_name<T>(), value);
}

template<class T>
[[noreturn]] void throw_field_to_njson_error(std::string_view field_name, std::string_view reason) {
    throw_field_to_njson_error(sn::type_name<T>(), field_name, reason);
}

template<class T>
[[noreturn]] void throw_field_from_njson_error(std::string_view field_name, std::string_view reason) {
    throw_field_from_njson_error(sn::type_name<T>(), field_name, reason);
}

template<class T>
[[noreturn]] void throw_missing_field_from_njson_error(std::string_view field_name) {
    throw_missing_field_from_njson_error(sn::type_name<T>(), field_name);
}

} // namespace sn::detail
