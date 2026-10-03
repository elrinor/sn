#pragma once

#include <array>
#include <utility> // For std::pair.
#include <type_traits> // For std::type_identity.
#include <string_view>

#include "sn/core/preprocessor.h"
#include "sn/core/tag.h"

namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] constexpr const auto &do_reflect_enum(Tags... tags) noexcept {
    return reflect_enum(std::type_identity<T>(), tags...);
}

} // namespace sn::detail

namespace sn {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] constexpr const auto &reflect_enum(Tags... tags) noexcept {
    return sn::detail::do_reflect_enum<T>(tags...);
}

} // namespace sn

#define SN_DEFINE_ENUM_REFLECTION(T, MAPPING, ... /* TAGS */)                                                           \
    [[nodiscard]] constexpr const auto &reflect_enum(std::type_identity<T> __VA_OPT__(,) __VA_ARGS__) noexcept {        \
        static constexpr auto value = std::to_array<std::pair<T, std::string_view>> MAPPING;                            \
        return value;                                                                                                   \
    }
