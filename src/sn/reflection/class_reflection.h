#pragma once

#include <initializer_list>
#include <string_view>
#include <type_traits>
#include <utility>
#include <tuple>

#include "sn/core/preprocessor.h"
#include "sn/core/tag.h"
#include "sn/reflection/detail/member_reflection.h"

#include "class_reflection_data.h"


namespace sn::detail {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] constexpr const auto &do_reflect_class(Tags... tags) noexcept {
    return reflect_class(std::type_identity<T>(), tags...);
}

} // namespace sn::detail

namespace sn {

template<class T, sn::concepts::tag... Tags>
[[nodiscard]] constexpr const auto &reflect_class(Tags... tags) noexcept {
    return sn::detail::do_reflect_class<T>(tags...);
}

} // namespace sn


// TODO(elric): #cpp23 the magic below with _class_reflection_container isn't needed in c++23, can just create a static
//              constexpr variable inside the function.

#define SN_DEFINE_CLASS_REFLECTION(T, CLASS_REFLECTION, ... /* TAGS */)                                                 \
    template<class...>                                                                                                  \
    struct _class_reflection_container;                                                                                 \
                                                                                                                        \
    template<>                                                                                                          \
    struct _class_reflection_container<T __VA_OPT__(,) __VA_ARGS__> {                                                   \
        static constexpr auto value = sn::class_reflection SN_PP_TUPLE_TRANSFORM(_SN_DEFINE_CLASS_REFLECTION_I, CLASS_REFLECTION); \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] constexpr const auto &reflect_class(std::type_identity<T> __VA_OPT__(,) __VA_ARGS__) noexcept {       \
        return _class_reflection_container<T __VA_OPT__(,) __VA_ARGS__>::value;                                         \
    }

#define _SN_DEFINE_CLASS_REFLECTION_I(FIELD_REFLECTION)                                                                 \
    _SN_DEFINE_CLASS_REFLECTION_II FIELD_REFLECTION

#define _SN_DEFINE_CLASS_REFLECTION_II(ARG0, ARG1, ...)                                                                 \
    sn::detail::make_member_reflection<ARG0, sn::detail::member_ptr_or_nullptr(ARG1)>(ARG1 __VA_OPT__(,) __VA_ARGS__)












