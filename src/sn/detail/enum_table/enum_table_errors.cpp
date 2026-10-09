#include "enum_table_errors.h"

#include <string>

#include "sn/core/error.h"
#include "sn/detail/format/format.h"
#include "sn/string/detail/string_errors.h"

namespace sn::detail {

void report_enum_to_string_error(std::string_view (*type_name)(), std::uint64_t value, bool is_signed, sn::error *err) {
    if (!err)
        return;

    // This static_cast relies on implementation-defined behavior, but it's symmetric to what we have in
    // type_erase_enum_reflection(), so it's OK.
    std::string value_string = is_signed ? sn::detail::format("{}", static_cast<std::int64_t>(value)) : sn::detail::format("{}", value);
    *err = sn::error(sn::detail::format("Cannot serialize provided enum value '{}' of type '{}' to string", value_string, type_name()));
}

void report_enum_from_string_error(std::string_view (*type_name)(), std::string_view value, sn::error *err) {
    if (err)
        report_from_string_error(*err, type_name(), value);
}

} // namespace sn::detail
