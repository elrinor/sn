#include "enum_table_exceptions.h"

#include "sn/core/exception.h"
#include "sn/string/detail/string_exceptions.h"

namespace sn::detail {

void throw_enum_to_string_error(std::string_view type_name, std::string_view value) {
    throw sn::exception("Cannot serialize provided enum value '{}' of type '{}' to string", value, type_name);
}

void throw_enum_from_string_error(std::string_view type_name, std::string_view value) {
    throw_from_string_error(type_name, value);
}

} // namespace sn::detail

