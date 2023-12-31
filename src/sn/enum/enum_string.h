#pragma once

#include <string>
#include <string_view>

#if SN_USE_STD_ENUM_HASH
#   include "std_enum_table.h"
#elif SN_USE_FROZEN_ENUM_HASH
#   include "frozen_enum_table.h"
#else
#   error "Enum hash library not configured"
#endif

#include "enum_reflection.h"

#include "sn/string/string.h"

// TODO(elric): #cpp23 the magic below with _enum_table_container isn't needed in c++23, can just create a static
//              constexpr variable inside a function.

#define _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, ATTRIBUTES, ... /* TAGS */)                          \
    template<class...>                                                                                                  \
    struct _enum_table_container;                                                                                       \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__> {                                                      \
        static constexpr auto reflection = sn::reflect_enum<ENUM __VA_OPT__(,) __VA_ARGS__>();                          \
        _SN_DEFINE_ENUM_STRING_TABLE(value, ENUM, CASE_SENSITIVITY, reflection)                                         \
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
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, [[]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_INLINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, inline __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_STATIC_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, static __VA_OPT__(,) __VA_ARGS__)
