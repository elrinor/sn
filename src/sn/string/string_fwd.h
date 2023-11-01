#pragma once

#include <string>
#include <string_view>

#include <sn/core/preprocessor.h> // For __VA_OPT__.

#define SN_DECLARE_TO_STRING_FUNCTION(TYPE, ... /* TAGS */)                                                             \
    [[nodiscard]] bool try_to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept;             \
    void to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_FROM_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                          \
    [[nodiscard]] bool try_from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;             \
    void from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

#define SN_DECLARE_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                               \
    SN_DECLARE_TO_STRING_FUNCTION(TYPE __VA_OPT__(,) __VA_ARGS__)                                                       \
    SN_DECLARE_FROM_STRING_FUNCTIONS(TYPE __VA_OPT__(,) __VA_ARGS__)
