#pragma once

#include <string>
#include <string_view>
#include <type_traits>

#include "sn/core/preprocessor.h" // For __VA_OPT__.

#define _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, TYPE_ARG, NORMAL_ATTRIBUTES, NODISCARD_ATTRIBUTES, ... /* TAGS */)         \
    NODISCARD_ATTRIBUTES bool try_to_string(TYPE_ARG src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept;         \
    NORMAL_ATTRIBUTES void to_string(TYPE_ARG src, std::string *dst __VA_OPT__(,) __VA_ARGS__);                         \
    NODISCARD_ATTRIBUTES bool try_from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept;      \
    NORMAL_ATTRIBUTES void from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Same as `SN_DECLARE_STRING_FUNCTIONS`, but `to_string` and `try_to_string` take `TYPE` by value. This can result in
 * better codegen on most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */)                                                     \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, TYPE, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

/**
 * Generates `sn` string function declarations for `TYPE` with tag types passed in varargs.
 *
 * The following declarations will be generated:
 * ```
 * [[nodiscard]] bool try_to_string(const TYPE &src, std::string *dst) noexcept;
 * void to_string(const TYPE &src, std::string *dst);
 * [[nodiscard]] bool try_from_string(std::string_view src, TYPE *dst) noexcept;
 * void from_string(std::string_view src, TYPE *dst);
 * ```
 *
 * If you passed any tag types to this macro, then they will be appended as additional arguments to the string
 * functions. Note that tags are always passed by value.
 *
 * A typical way to use this macro is:
 * - Invoke it in a header file for your type.
 * - Implement all functions in the cpp file.
 *
 * @param TYPE                          Type to generate `sn` string function declarations for.
 * @param ...                           Tags, if any.
 */
#define SN_DECLARE_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                               \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, const TYPE &, [[]], [[nodiscard]] __VA_OPT__(,) __VA_ARGS__)

/**
 * Same as `SN_DECLARE_STRING_FUNCTIONS`, but declares friend functions. To be used inside a class definition.
 *
 * @see SN_DECLARE_STRING_FUNCTIONS
 */
#define SN_DECLARE_FRIEND_STRING_FUNCTIONS(TYPE, ... /* TAGS */)                                                        \
    _SN_DECLARE_STRING_FUNCTIONS_I(TYPE, const TYPE &, friend, friend __VA_OPT__(,) __VA_ARGS__) // Can't have [[nodiscard]] on a friend function declaration...
