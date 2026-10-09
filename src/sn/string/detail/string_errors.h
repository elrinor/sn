#pragma once

#include <system_error>
#include <string_view>

#include "sn/core/error_fwd.h"
#include "sn/core/type_name.h"

namespace sn::detail {

void report_from_string_error(sn::error &err, std::string_view type_name, std::string_view value);
void report_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::string_view reason);
void report_number_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::errc error);

template<class T>
void report_from_string_error(sn::error *err, std::string_view value) {
    if (err) [[unlikely]]
        report_from_string_error(*err, sn::type_name<T>(), value);
}

template<class T>
void report_from_string_error(sn::error *err, std::string_view value, std::string_view reason) {
    if (err) [[unlikely]]
        report_from_string_error(*err, sn::type_name<T>(), value, reason);
}

template<class T>
void report_number_from_string_error(sn::error *err, std::string_view value, std::errc error) {
    if (err) [[unlikely]]
        report_number_from_string_error(*err, sn::type_name<T>(), value, error);
}

} // namespace sn::detail
