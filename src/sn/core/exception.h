#pragma once

#include <utility>
#include <stdexcept>

#include "format.h"

namespace sn {

class exception : public std::runtime_error {
public:
    template<class... Args>
    exception(sn::detail::format_string<Args...> fmt, Args &&... args) : std::runtime_error(sn::detail::format(fmt, std::forward<Args>(args)...)) {}
};

} // namespace sn
