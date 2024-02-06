#pragma once

#include <QtCore/QString>
#include <QtCore/QStringView>

#include "sn/core/preprocessor.h"
#include "sn/enum/enum_reflection.h"
#include "sn/qstring/qstring.h"
#include "sn/qstring/qstring_fwd.h"
#include "sn/detail/preprocessor/preprocessor.h"

#include "std_qstring_enum_table.h"

#define _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, TABLE_DEFINITION_MACRO, ATTRIBUTES, ... /* TAGS */) \
    _SN_ENABLE_QSTRING_FUNCTIONS(ENUM, ATTRIBUTES __VA_OPT__(,) __VA_ARGS__)                                            \
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
    [[nodiscard]] ATTRIBUTES bool try_to_qstring(const ENUM &src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept {    \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.try_to_string(src, dst);                    \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void to_qstring(const ENUM &src, QString *dst __VA_OPT__(,) __VA_ARGS__) {                               \
        _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.to_string(src, dst);                               \
    }                                                                                                                   \
                                                                                                                        \
    [[nodiscard]] ATTRIBUTES bool try_from_qstring(QStringView src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) noexcept {     \
        return _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.try_from_string(src, dst);                  \
    }                                                                                                                   \
                                                                                                                        \
    ATTRIBUTES void from_qstring(QStringView src, ENUM *dst __VA_OPT__(,) __VA_ARGS__) {                                \
        _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__>::value.from_string(src, dst);                             \
    }

#define SN_DEFINE_ENUM_QSTRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                        \
    _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_QSTRING_TABLE, [[]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_INLINE_ENUM_QSTRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                 \
    _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_QSTRING_TABLE, inline __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_STATIC_ENUM_QSTRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                 \
    _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, _SN_DEFINE_ENUM_QSTRING_TABLE, static __VA_OPT__(,) __VA_ARGS__)
