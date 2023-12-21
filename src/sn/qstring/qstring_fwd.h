#pragma once

#include <QtCore/QString>
#include <QtCore/QStringView>

#include "sn/core/preprocessor.h" // For __VA_OPT__.

#define SN_DECLARE_TO_QSTRING_FUNCTION(TYPE, ... /* TAGS */)                                                             \
    [[nodiscard]] bool try_to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                 \
    void to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_FROM_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                          \
    [[nodiscard]] bool try_from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                  \
    void from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                               \
    SN_DECLARE_TO_QSTRING_FUNCTION(TYPE __VA_OPT__(,) __VA_ARGS__)                                                       \
    SN_DECLARE_FROM_QSTRING_FUNCTIONS(TYPE __VA_OPT__(,) __VA_ARGS__)
