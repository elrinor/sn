#pragma once

#include <system_error>

#include <sn/core/type_name.h>

namespace sn {

[[noreturn]] void throw_to_string_error(std::string_view type_name);
[[noreturn]] void throw_from_string_error(std::string_view value, std::string_view type_name);
[[noreturn]] void throw_number_from_string_error(std::string_view value, std::string_view type_name, std::errc error);

} // namespace sn
