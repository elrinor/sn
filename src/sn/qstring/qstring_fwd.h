#pragma once

#include <QtCore/QString>
#include <QtCore/QStringView>

#include "sn/core/preprocessor.h" // For __VA_OPT__.

/**
 * @internal
 * @see _SN_ENABLE_STRING_FUNCTIONS
 */
#define _SN_ENABLE_QSTRING_FUNCTIONS(TYPE, ATTRIBUTES, ... /* TAGS */)                                                  \
    ATTRIBUTES void is_qstring_supported_type(std::type_identity<TYPE> __VA_OPT__(,) __VA_ARGS__);

#define _SN_DECLARE_QSTRING_FUNCTIONS_I(TYPE, TYPE_ARG, NORMAL_ATTRIBUTES, NODISCARD_ATTRIBUTES, ... /* TAGS */)        \
    _SN_ENABLE_QSTRING_FUNCTIONS(TYPE, NORMAL_ATTRIBUTES __VA_OPT__(,) __VA_ARGS__);                                    \
    NODISCARD_ATTRIBUTES bool try_to_qstring(TYPE_ARG src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept;            \
    NORMAL_ATTRIBUTES void to_qstring(TYPE_ARG src, QString *dst __VA_OPT__(,) __VA_ARGS__);                            \
    NODISCARD_ATTRIBUTES bool try_from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;          \
    NORMAL_ATTRIBUTES void from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 * @see _SN_DECLARE_STRING_FUNCTIONS_BY_VALUE
 */
#define _SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */)                                                    \
    _SN_DECLARE_QSTRING_FUNCTIONS_I(TYPE, TYPE, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                              \
    _SN_DECLARE_QSTRING_FUNCTIONS_I(TYPE, const TYPE &, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_FRIEND_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                       \
    _SN_DECLARE_QSTRING_FUNCTIONS_I(TYPE, const TYPE &, friend, friend __VA_OPT__(,) __VA_ARGS__) // Can't have [[nodiscard]] on a friend function declaration...
