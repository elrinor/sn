#include "string_errors.h"

#include "sn/core/error.h"
#include "sn/detail/format/format.h"

namespace sn::detail {

void report_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::string_view reason) {
    if (reason.empty()) {
        err = sn::error(sn::detail::format("Cannot deserialize '{}' as '{}'", value, type_name));
    } else {
        err = sn::error(sn::detail::format("Cannot deserialize '{}' as '{}': {}", value, type_name, reason));
    }
}

void report_number_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::errc error) {
    if (error == std::errc::invalid_argument) {
        err = sn::error(sn::detail::format("'{}' is not a number", value));
    } else if (error == std::errc::result_out_of_range) {
        err = sn::error(sn::detail::format("'{}' does not fit in the range of {}", value, type_name));
    } else {
        err = sn::error(sn::detail::format("{}: {}", value, std::make_error_code(error).message())); // TODO(elric): revisit error messages here.
    }
}

} // namespace sn::detail
