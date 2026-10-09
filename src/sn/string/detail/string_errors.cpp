#include "string_errors.h"

#include "sn/core/error.h"
#include "sn/core/detail/concat.h"

namespace sn::detail {

void report_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::string_view reason) {
    if (reason.empty()) {
        err = sn::error(sn::detail::concat("Cannot deserialize '", value, "' as '", type_name, "'"));
    } else {
        err = sn::error(sn::detail::concat("Cannot deserialize '", value, "' as '", type_name, "': ", reason));
    }
}

void report_number_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::errc error) {
    if (error == std::errc::invalid_argument) {
        err = sn::error(sn::detail::concat("'", value, "' is not a number"));
    } else if (error == std::errc::result_out_of_range) {
        err = sn::error(sn::detail::concat("'", value, "' does not fit in the range of ", type_name));
    } else {
        err = sn::error(sn::detail::concat(value, ": ", std::make_error_code(error).message())); // TODO(elric): revisit error messages here.
    }
}

} // namespace sn::detail
