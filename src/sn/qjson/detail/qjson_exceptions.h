#pragma once

#include <string_view>

#include "sn/core/type_name.h"

class QJsonValue;

namespace sn::detail {

[[noreturn]] void throw_from_qjson_error(std::string_view type_name, const QJsonValue &value);

template<class T>
[[noreturn]] void throw_from_qjson_error(const QJsonValue &value) {
    throw_from_qjson_error(sn::type_name<T>(), value);
}

} // namespace sn::detail
