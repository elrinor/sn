#pragma once

#include <system_error>
#include <string_view>

#include "sn/core/error_fwd.h"
#include "sn/core/type_name.h"

namespace sn::detail {

void report_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::string_view reason = {});
void report_number_from_string_error(sn::error &err, std::string_view type_name, std::string_view value, std::errc error);

// These take the arguments of the from_string call that failed. dst is only used to deduce T.

template<class T>
void report_from_string_error(std::string_view src, [[maybe_unused]] T *dst, sn::error *err, std::string_view reason = {}) {
    if (err) [[unlikely]]
        report_from_string_error(*err, sn::type_name<T>(), src, reason);
}

template<class T>
void report_number_from_string_error(std::string_view src, [[maybe_unused]] T *dst, sn::error *err, std::errc error) {
    if (err) [[unlikely]]
        report_number_from_string_error(*err, sn::type_name<T>(), src, error);
}

} // namespace sn::detail
