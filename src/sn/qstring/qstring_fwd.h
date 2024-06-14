#pragma once

#include "sn/core/preprocessor.h" // For __VA_OPT__.

class QString;
class QStringView;

/**
 * @internal
 * @see _SN_DECLARE_STRING_FUNCTIONS_BY_VALUE
 */
#define _SN_DECLARE_QSTRING_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */)                                                    \
    [[nodiscard]] bool try_to_qstring(TYPE src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                       \
    void to_qstring(TYPE src, QString *dst __VA_OPT__(,) __VA_ARGS__);                                                  \
    [[nodiscard]] bool try_from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                 \
    void from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                              \
    [[nodiscard]] bool try_to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                \
    void to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__);                                           \
    [[nodiscard]] bool try_from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                 \
    void from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_FRIEND_QSTRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                       \
    friend bool try_to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                       \
    friend void to_qstring(const TYPE &src, QString *dst __VA_OPT__(,) __VA_ARGS__);                                    \
    friend bool try_from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;                        \
    friend void from_qstring(QStringView src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);
