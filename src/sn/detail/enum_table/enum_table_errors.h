#pragma once

#include <cstdint>
#include <string_view>

#include "sn/core/error_fwd.h"

namespace sn::detail {

// These do nothing if err is nullptr. They take type_name as a function so that it's not called in this case.
void report_enum_to_string_error(std::string_view (*type_name)(), std::uint64_t value, bool is_signed, sn::error *err);
void report_enum_from_string_error(std::string_view (*type_name)(), std::string_view value, sn::error *err);

} // namespace sn::detail
