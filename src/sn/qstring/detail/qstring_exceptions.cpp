#include "qstring_exceptions.h"

#include "sn/string/string_exceptions.h"

namespace sn::detail {

void throw_from_string_error(std::string_view type_name, QStringView value) {
    throw_from_string_error(type_name, std::string_view(value.toUtf8()));
}

} // namespace sn::detail
