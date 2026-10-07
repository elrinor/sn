#pragma once

#include <cstdint>
#include <string_view>

#include "sn/core/error_fwd.h"

namespace sn::detail {

void report_enum_to_string_error(sn::error &err, std::string_view type_name, std::uint64_t value, bool is_signed);
void report_enum_from_string_error(sn::error &err, std::string_view type_name, std::string_view value);

} // namespace sn::detail
