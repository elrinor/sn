#pragma once

#include <string>
#include <string_view>

#include "sn/core/preprocessor.h"
#include "sn/reflection/enum_reflection.h"
#include "sn/detail/codegen/tuple_types.h"
#include "sn/string/detail/frozen_enum_table.h"

#include "string.h"
#include "string_fwd.h"

#define _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, TABLE_DEFINITION_MACRO, ATTRIBUTES, ... /* TAGS */)  \
    [[nodiscard]] ATTRIBUTES const auto &_enum_table(std::type_identity<ENUM> __VA_OPT__(,) __VA_ARGS__) {              \
        static constexpr auto reflection = sn::reflect_enum<ENUM> _SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__));    \
        TABLE_DEFINITION_MACRO(value, ENUM, CASE_SENSITIVITY, reflection)                                               \
        return value;                                                                                                   \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_to_string(const ENUM &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return _SN_ENUM_STRING_TABLE(ENUM __VA_OPT__(,) __VA_ARGS__).try_to_string(src, dst);                           \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void to_string(const ENUM &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) {                            \
        _SN_ENUM_STRING_TABLE(ENUM __VA_OPT__(,) __VA_ARGS__).to_string(src, dst);                                      \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_from_string(std::string_view src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) noexcept { \
        return _SN_ENUM_STRING_TABLE(ENUM __VA_OPT__(,) __VA_ARGS__).try_from_string(src, dst);                         \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void from_string(std::string_view src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) {                            \
        _SN_ENUM_STRING_TABLE(ENUM __VA_OPT__(,) __VA_ARGS__).from_string(src, dst);                                    \
    }

/**
 * @internal
 *
 * Expands to a call to `_enum_table` that was defined by `_SN_DEFINE_ENUM_STRING_FUNCTIONS_I`.
 *
 * Note the parentheses around the function name, they disable ADL. We always want the table that was defined in the
 * current namespace, even if another one can be found in the enum's namespace.
 */
#define _SN_ENUM_STRING_TABLE(ENUM, ... /* TAGS */)                                                                     \
    (_enum_table)(std::type_identity<ENUM>() SN_PP_TUPLE_ENUM_TRAILING(_SN_PP_TUPLE_TYPES_TO_DEFALT_CTORS((__VA_ARGS__))))

#define SN_DEFINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                         \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_STRING_TABLE, [[]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_INLINE_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_STRING_TABLE, inline __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_STATIC_ENUM_STRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                  \
    _SN_DEFINE_ENUM_STRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_STRING_TABLE, static __VA_OPT__(,) __VA_ARGS__)
