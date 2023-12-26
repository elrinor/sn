#pragma once

#include <system_error>
#include <string_view>

#include <QtCore/QStringView>

#include "sn/core/type_name.h"

namespace sn {

[[noreturn]] void throw_from_string_error(std::string_view type_name, QStringView value);
[[noreturn]] void throw_number_from_string_error(std::string_view type_name, QStringView value, std::errc error);

template<class T>
[[noreturn]] void throw_from_string_error(QStringView value) {
    throw_from_string_error(sn::type_name<T>(), value);
}

template<class T>
[[noreturn]] void throw_number_from_string_error(QStringView value, std::errc error) {
    throw_number_from_string_error(sn::type_name<T>(), value, error);
}

} // namespace sn
