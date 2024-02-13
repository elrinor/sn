#pragma once

#include <string>
#include <string_view>

#include "sn/core/enum_reflection.h"
#include "sn/core/preprocessor.h"
#include "sn/detail/preprocessor/preprocessor.h"

#if SN_USE_STD_ENUM_HASH
#   include "sn/string/detail/std_enum_table.h"
#elif SN_USE_FROZEN_ENUM_HASH
#   include "sn/string/detail/frozen_enum_table.h"
#else
#   error "Enum hash library not configured"
#endif

#include "string.h"
#include "string_fwd.h"

// TODO(elric): #cpp23 the magic below with _enum_table_container isn't needed in c++23, can just create a static
//              constexpr variable inside a function.

#define _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, TABLE_DEFINITION_MACRO, ATTRIBUTES, ... /* TAGS */)  \
    _SN_ENABLE_STRING_FUNCTIONS(ENUM, ATTRIBUTES __VA_OPT__(,) __VA_ARGS__);                                            \
                                                                                                                        \
    template<class...>                                                                                                  \
    struct _enum_table_container;                                                                                       \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__> {                                                      \
        static constexpr auto reflection = sn::reflect_enum<ENUM>(__VA_OPT__(SN_PP_TUPLE_FOR_EACH(_SN_PP_APPEND_PARENS, (__VA_ARGS__)))); \
        TABLE_DEFINITION_MACRO(value, ENUM, CASE_SENSITIVITY, reflection)                                               \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_to_string(const ENUM &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.try_to_string(src, dst);                    \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void to_string(const ENUM &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) {                            \
        _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.to_string(src, dst);                               \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_from_string(std::string_view src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.try_from_string(src, dst);                  \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void from_string(std::string_view src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) {                            \
        _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.from_string(src, dst);                             \
    }

#define SN_DEFINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                         \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_STRING_TABLE, [[]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_INLINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_STRING_TABLE, inline __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_STATIC_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_STRING_TABLE, static __VA_OPT__(,) __VA_ARGS__)
