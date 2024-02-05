#pragma once

#include <string>
#include <string_view>

#include "sn/core/preprocessor.h" // For __VA_OPT__.

#define SN_DECLARE_STRING_SUPPORTED_TYPE(TYPE, ... /* TAGS */)

#define _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, NORMAL_ATTRIBUTES, NODISCARD_ATTRIBUTES, ... /* TAGS */)                   \
    NODISCARD_ATTRIBUTES bool try_to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept;      \
    NORMAL_ATTRIBUTES void to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__);                      \
    NODISCARD_ATTRIBUTES bool try_from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;      \
    NORMAL_ATTRIBUTES void from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                               \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

#define SN_DECLARE_FRIEND_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                        \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, friend, friend __VA_OPT__(,) __VA_ARGS__) // Can't have [[nodiscard]] on a friend function declaration...
