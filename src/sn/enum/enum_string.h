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

#include "sn/string/string_exceptions.h"

#include "enum_reflection.h"

#include "sn/string/string.h"

// TODO(elric): #cpp23 the magic below with _enum_table_container isn't needed in c++23, can just create a static
//              constexpr variable inside the function.

#define SN_DEFINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                         \
    template<class...>                                                                                                  \
    struct _enum_table_container;                                                                                       \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__> { /* NOLINT */                                         \
        static inline const auto value = sn::detail::std_enum_table<ENUM, CASE_SENSITIVITY>(sn::reflect_enum<ENUM __VA_OPT__(,) __VA_ARGS__>()); /* NOLINT */ \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] bool try_to_string(const ENUM &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept { /* NOLINT */ \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.try_to_string(src, dst); /* NOLINT */       \
    }                                                                                                                   \
                                                                                                                        \
    void to_string(const ENUM &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) { /* NOLINT */                          \
        _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.to_string(src, dst); /* NOLINT */                  \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] bool try_from_string(std::string_view src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) noexcept { /* NOLINT */ \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.try_from_string(src, dst); /* NOLINT */     \
    }                                                                                                                   \
                                                                                                                        \
    void from_string(std::string_view src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) { /* NOLINT */                          \
        _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.from_string(src, dst); /* NOLINT */                \
    }
