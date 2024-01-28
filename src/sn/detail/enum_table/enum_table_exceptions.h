#pragma once

#include <string_view>

namespace sn {

[[noreturn]] void throw_enum_to_string_error(std::string_view type_name, std::string_view value);
[[noreturn]] void throw_enum_from_string_error(std::string_view type_name, std::string_view value);

} // namespace sn
