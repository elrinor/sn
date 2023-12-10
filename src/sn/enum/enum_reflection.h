#pragma once

#include <initializer_list>
#include <string_view>
#include <utility>
#include <type_traits>
#include <concepts>

#include "sn/core/preprocessor.h"

namespace sn::detail {

// TODO(elric): #cpp20 use std::string_view instead of const char * here when we upgrade to gcc that's sane and doesn't
//              choke here.
template<class T>
[[nodiscard]] constexpr std::initializer_list<std::pair<T, const char *>> do_reflect_enum() noexcept {
    return reflect_enum(std::type_identity<T>());
}

} // namespace sn::detail

namespace sn {

template<class T>
[[nodiscard]] constexpr std::initializer_list<std::pair<T, const char *>> reflect_enum() noexcept {
    return sn::detail::do_reflect_enum<T>();
}

} // namespace sn

// TODO(elric): #cpp23 the magic below with _enum_reflection_container isn't needed in c++23, can just create a static
//              constexpr variable inside the function.

#define SN_DEFINE_ENUM_REFLECTION(T, ... /* MAPPING */)                                                                 \
    template<class T>                                                                                                   \
    struct _enum_reflection_container;                                                                                  \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_reflection_container<T> {                                                                              \
        static constexpr std::initializer_list<std::pair<T, const char *>> value = __VA_ARGS__;                         \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] constexpr auto reflect_enum(std::type_identity<T>) noexcept {                                         \
        return _enum_reflection_container<T>::value;                                                                    \
    }
