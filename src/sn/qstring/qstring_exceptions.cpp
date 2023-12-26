#include "qstring_exceptions.h"

#include "sn/string/string_exceptions.h"

namespace sn {

void throw_from_string_error(std::string_view type_name, QStringView value) {
    sn::throw_from_string_error(type_name, std::string_view(value.toUtf8()));
}

void throw_number_from_string_error(std::string_view type_name, QStringView value, std::errc error) {
    sn::throw_number_from_string_error(type_name, std::string_view(value.toUtf8()), error);
}

} // namespace sn
