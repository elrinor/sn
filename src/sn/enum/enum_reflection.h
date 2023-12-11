#pragma once

#include <initializer_list>
#include <utility> // For std::pair.
#include <type_traits> // For std::type_identity.

namespace sn::detail {

// TODO(elric): #cpp20 use std::string_view instead of const char * here when we upgrade to gcc 12.3+ that's sane and
//              doesn't choke here. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102921.
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
