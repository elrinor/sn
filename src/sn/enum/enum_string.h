#pragma once

#include <string>
#include <string_view>

#if SN_USE_STD_ENUM_HASH
#   include "std_enum_table.h"
#elif SN_USE_FROZEN_ENUM_HASH
#   error "Frozen support for enum string functions is not implemented yet"
#else
#   error "Enum hash library not configured"
#endif

#include "sn/core/preprocessor.h"
#include "sn/string/string_exceptions.h"

#include "enum_reflection.h"
#include "enum_exceptions.h"

// TODO(elric): #cpp23 the magic below with _enum_table_container isn't needed in c++23, can just create a static
//              constexpr variable inside the function.

#define SN_DEFINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY)                                                         \
    template<class T>                                                                                                   \
    struct _enum_table_container;                                                                                       \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_table_container<ENUM> {                                                                                \
        static inline const auto value = sn::detail::std_enum_table<ENUM, CASE_SENSITIVITY>(sn::reflect_enum<ENUM>());  \
    };                                                                                                                  \
                                                                                                                        \
    bool try_to_string(const ENUM &src, std::string *dst) {                                                             \
        return _enum_table_container<ENUM>::value.try_to_string(src, dst);                                              \
    }                                                                                                                   \
                                                                                                                        \
    bool try_from_string(std::string_view src, ENUM *dst) {                                                             \
        return _enum_table_container<ENUM>::value.try_from_string(src, dst);                                            \
    }                                                                                                                   \
                                                                                                                        \
    void to_string(const ENUM &src, std::string *dst) {                                                                 \
        if (!_enum_table_container<ENUM>::value.try_to_string(src, dst))                                                \
            sn::throw_enum_to_string_error<ENUM>(src);                                                                  \
    }                                                                                                                   \
                                                                                                                        \
    void from_string(std::string_view src, ENUM *dst) {                                                                 \
        if (!_enum_table_container<ENUM>::value.try_from_string(src, dst))                                              \
            sn::throw_enum_from_string_error<ENUM>(src);                                                                \
    }
