#pragma once

#include <QtCore/QString>
#include <QtCore/QStringView>

#include "sn/qstring/qstring.h"
#include "sn/enum/enum_reflection.h"

#include "std_qstring_enum_table.h"

#define _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, ATTRIBUTES, ... /* TAGS */)                         \
    template<class...>                                                                                                  \
    struct _enum_table_container;                                                                                       \
                                                                                                                        \
    template<>                                                                                                          \
    struct _enum_table_container<ENUM __VA_OPT__(,) __VA_ARGS__> {                                                      \
        static constexpr auto reflection = sn::reflect_enum<ENUM __VA_OPT__(,) __VA_ARGS__>();                          \
        _SN_DEFINE_ENUM_QSTRING_TABLE(value, ENUM, CASE_SENSITIVITY, reflection)                                        \
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
    _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, [[]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_INLINE_ENUM_QSTRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                 \
    _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, inline __VA_OPT__(,) __VA_ARGS__)

#define SN_DEFINE_STATIC_ENUM_QSTRING_FUNCTIONS(ENUM, CASE_SENSITIVITY, ... /* TAGS */)                                 \
    _SN_DEFINE_ENUM_QSTRING_FUNCTIONS_I(ENUM, CASE_SENSITIVITY, static __VA_OPT__(,) __VA_ARGS__)
