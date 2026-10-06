#pragma once

#include <string>
#include <string_view>

#include "sn/core/preprocessor.h"
#include "sn/reflection/enum_reflection.h"
#include "sn/detail/codegen/tuple_types.h"
#include "sn/string/detail/string_enum_table.h"

#include "string.h"
#include "string_fwd.h"

// TODO(elric): #cpp23 the magic below with _enum_table_container isn't needed in c++23, can just create a static
//              constexpr variable inside a function once we have P2647.

#define _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, OPTIONS, TABLE_DEFINITION_MACRO, ATTRIBUTES, ... /* TAGS */)           \
    template<class...>                                                                                                  \
    struct _enum_table_container;                                                                                       \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__> {                                                      \
        static constexpr auto reflection = sn::reflect_enum<ENUM> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__));    \
        TABLE_DEFINITION_MACRO(value, ENUM, OPTIONS, reflection)                                                        \
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

/**
 * Generates `std::string` function definitions for `ENUM` with tag types passed in varargs. The functions are the
 * ones that `SN_DECLARE_STRING_FUNCTIONS` declares.
 *
 * Conversions are done with lookup tables that are built at compile time from the reflection of `ENUM`. The reflection
 * should be defined with `SN_DEFINE_ENUM_REFLECTION` for the same tag types.
 *
 * @param ENUM                          Enum to generate `std::string` function definitions for.
 * @param OPTIONS                       Either `sn::case_sensitive` or `sn::case_insensitive`, this is how strings are
 *                                      matched in `from_string`. Can be combined with a kind of the lookup table to
 *                                      use in `to_string`, e.g. `sn::case_sensitive | sn::hashed_enum_table`. If the
 *                                      kind is not specified, then a suitable one is picked automatically.
 * @param ...                           Tags, if any.
 * @see SN_DECLARE_STRING_FUNCTIONS
 * @see SN_DEFINE_ENUM_REFLECTION
 */
#define SN_DEFINE_ENUM_STRING_FUNCTIONS(ENUM, OPTIONS, ... /* TAGS */)                                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, OPTIONS, _SN_DEFINE_ENUM_STRING_TABLE, [[]] __VA_OPT__(,) __VA_ARGS__)

/**
 * Same as `SN_DEFINE_ENUM_STRING_FUNCTIONS`, but defines inline functions. To be used in a header file.
 *
 * @see SN_DEFINE_ENUM_STRING_FUNCTIONS
 */
#define SN_DEFINE_INLINE_ENUM_STRING_FUNCTIONS(ENUM, OPTIONS, ... /* TAGS */)                                           \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, OPTIONS, _SN_DEFINE_ENUM_STRING_TABLE, inline __VA_OPT__(,) __VA_ARGS__)

/**
 * Same as `SN_DEFINE_ENUM_STRING_FUNCTIONS`, but defines static functions.
 *
 * @see SN_DEFINE_ENUM_STRING_FUNCTIONS
 */
#define SN_DEFINE_STATIC_ENUM_STRING_FUNCTIONS(ENUM, OPTIONS, ... /* TAGS */)                                           \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, OPTIONS, _SN_DEFINE_ENUM_STRING_TABLE, static __VA_OPT__(,) __VA_ARGS__)
