#pragma once

#include <array>
#include <utility> // For std::pair.
#include <type_traits> // For std::type_identity.
#include <string_view>

#include "sn/core/preprocessor.h"

namespace sn::detail {

template<class T, class... Tags>
[[nodiscard]] constexpr const auto &do_reflect_enum() noexcept {
    return reflect_enum(std::type_identity<T>(), Tags()...);
}

} // namespace sn::detail

namespace sn {

template<class T, class... Tags>
[[nodiscard]] constexpr const auto &reflect_enum() noexcept {
    return sn::detail::do_reflect_enum<T, Tags...>();
}

} // namespace sn

// TODO(elric): #cpp23 the magic below with _enum_reflection_container isn't needed in c++23, can just create a static
//              constexpr variable inside the function.

#define SN_DEFINE_ENUM_REFLECTION(T, MAPPING, ... /* TAGS */)                                                           \
    template<class...>                                                                                                  \
    struct _enum_reflection_container;                                                                                  \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_reflection_container<T __VA_OPT__(,) __VA_ARGS__> {                                                    \
        static constexpr auto value = std::to_array<std::pair<T, std::string_view>>(SN_PP_REMOVE_PARENS(MAPPING));      \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] constexpr const auto &reflect_enum(std::type_identity<T> __VA_OPT__(,) __VA_ARGS__) noexcept {        \
        return _enum_reflection_container<T __VA_OPT__(,) __VA_ARGS__>::value;                                          \
    }
