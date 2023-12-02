#pragma once

#include <utility>
#include <string>

#if SN_USE_FMT_FORMAT
#   include <fmt/format.h>
#   define SN_FORMAT_NAMESPACE ::fmt
#elif SN_USE_STD_FORMAT
#   include <format>
#   define SN_FORMAT_NAMESPACE ::std
#else
#   error "Format library is not configured"
#endif

namespace sn::detail {

template<class... Args>
using format_string = SN_FORMAT_NAMESPACE::format_string<Args...>;

template<class... Args>
std::string format(format_string<Args...> fmt, Args &&... args) {
    return SN_FORMAT_NAMESPACE::format(fmt, std::forward<Args>(args)...);
}

} // namespace sn::detail
