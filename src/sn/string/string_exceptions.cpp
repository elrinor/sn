#include "string_exceptions.h"

#include <sn/core/exception.h>

namespace sn {

void throw_to_string_error(std::string_view type_name) {
    throw sn::exception("Cannot serialize provided value of type '{}' to string", type_name);
}

void throw_from_string_error(std::string_view value, std::string_view type_name) {
    throw sn::exception("Cannot deserialize '{}' as '{}'", value, type_name);
}

void throw_number_from_string_error(std::string_view value, std::string_view type_name, std::errc error) {
    if (error == std::errc::invalid_argument)
        throw sn::exception("'{}' is not a number", value);

    if (error == std::errc::result_out_of_range)
        throw sn::exception("'{}' does not fit in the range of {}", value, type_name);

    throw sn::exception("{}: {}", value, std::make_error_code(error).message());
}

} // namespace sn

