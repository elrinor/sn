#pragma once

#include "sn/core/preprocessor.h" // For __VA_OPT__.

class QJsonValue;

#define _SN_DECLARE_QJSON_FUNCTIONS_I(TYPE, TYPE_ARG, NORMAL_ATTRIBUTES, NODISCARD_ATTRIBUTES, ... /* TAGS */)          \
    NODISCARD_ATTRIBUTES bool try_to_qjson(TYPE_ARG src, QJsonValue *dst __VA_OPT__(,) __VA_ARGS__) noexcept;           \
    NORMAL_ATTRIBUTES void to_qjson(TYPE_ARG src, QJsonValue *dst __VA_OPT__(,) __VA_ARGS__);                           \
    NODISCARD_ATTRIBUTES bool try_from_qjson(const QJsonValue &src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;      \
    NORMAL_ATTRIBUTES void from_qjson(const QJsonValue &src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 * @see _SN_DECLARE_STRING_FUNCTIONS_BY_VALUE
 */
#define _SN_DECLARE_QJSON_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */)                                                      \
    _SN_DECLARE_QJSON_FUNCTIONS_I(TYPE, TYPE, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_QJSON_FUNCTIONS(TYPE, ... /* TAGS */)                                                                \
    _SN_DECLARE_QJSON_FUNCTIONS_I(TYPE, const TYPE &, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_FRIEND_QJSON_FUNCTIONS(TYPE, ... /* TAGS */)                                                         \
    _SN_DECLARE_QJSON_FUNCTIONS_I(TYPE, const TYPE &, friend, friend __VA_OPT__(,) __VA_ARGS__) // Can't have [[nodiscard]] on a friend function declaration...
