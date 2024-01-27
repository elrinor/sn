#pragma once

#include <string_view>

#include <QtCore/QStringView> // NOLINT.

#include "sn/core/type_name.h"

namespace sn {

[[noreturn]] void throw_from_string_error(std::string_view type_name, QStringView value);

template<class T>
[[noreturn]] void throw_from_string_error(QStringView value) {
    throw_from_string_error(sn::type_name<T>(), value);
}

} // namespace sn
