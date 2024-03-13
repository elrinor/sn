#pragma once

#include "sn/core/detail/type_name_builtins.h"

namespace sn::detail {

template<class T>
[[nodiscard]] std::string_view do_type_name() noexcept {
    using sn::builtins::type_name;
    return type_name(std::type_identity<T>());
}

} // namespace sn::detail

namespace sn {

template<class T>
[[nodiscard]] std::string_view type_name() noexcept {
    return sn::detail::do_type_name<T>();
}

} // namespace sn
