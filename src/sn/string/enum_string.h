#pragma once

#include <string>
#include <string_view>

#include "sn/core/error_fwd.h"
#include "sn/core/preprocessor.h"
#include "sn/reflection/enum_reflection.h"
#include "sn/detail/codegen/tuple_types.h"
#include "sn/string/detail/frozen_enum_table.h"

#include "string.h"
#include "string_fwd.h"

// TODO(elric): #cpp23 the magic below with _enum_table_container isn't needed in c++23, can just create a static
//              constexpr variable inside a function once we have P2647.

#define _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, ATTRIBUTES, ... /* TAGS */)                          \
    template<class...>                                                                                                  \
    struct _enum_table_container;                                                                                       \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__> {                                                      \
        static constexpr auto reflection = sn::reflect_enum<ENUM> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__));    \
        _SN_DEFINE_ENUM_STRING_TABLE(value, ENUM, CASE_SENSITIVITY, reflection)                                         \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool to_string(const ENUM &src, std::string *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__) { \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.to_string(src, dst, err);                  \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool from_string(std::string_view src, ENUM *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__) { \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.from_string(src, dst, err);                \
    }

#define SN_DEFINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                         \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, [[]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_INLINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, inline __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_STATIC_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, static __VA_OPT__(,) __VA_ARGS__)
