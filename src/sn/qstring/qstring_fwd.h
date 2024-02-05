#pragma once

#include <QtCore/QString>
#include <QtCore/QStringView>

#include "sn/core/preprocessor.h" // For __VA_OPT__.

#define _SN_DECLARE_QSTRING_FUNCTIONS_I(TYPE, NORMAL_ATTRIBUTES, NODISCARD_ATTRIBUTES, ... /* TAGS */)                  \
    NODISCARD_ATTRIBUTES bool try_to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept;         \
    NORMAL_ATTRIBUTES void to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__);                         \
    NODISCARD_ATTRIBUTES bool try_from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;          \
    NORMAL_ATTRIBUTES void from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                              \
    _SN_DECLARE_QSTRING_FUNCTIONS_I(TYPE, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_FRIEND_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                       \
    _SN_DECLARE_QSTRING_FUNCTIONS_I(TYPE, friend, friend __VA_OPT__(,) __VA_ARGS__) // Can't have [[nodiscard]] on a friend function declaration...
