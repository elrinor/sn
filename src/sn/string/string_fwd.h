//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once

#include <string>
#include <string_view>

#include "sn/core/error_fwd.h"
#include "sn/core/preprocessor.h" // For __VA_OPT__.

//
// A note on implementation.
//
// We used to have a single _SN_DECLARE_FUNCTIONS macro that did it all, and the macros in all *fwd.h headers were just
// invoking it. This was subsequently scrapped because we want the SN_DECLARE_* macros to be understandable for the
// people using them. People will come here, copy-paste the macro body, then paste it into their cpp file, and start
// implementing the functions.
//

/**
 * Generates `std::string` function declarations for `TYPE` with tag types passed in varargs.
 *
 * The following declarations will be generated:
 * ```
 * [[nodiscard]] bool to_string(const TYPE &src, std::string *dst, sn::error *err);
 * [[nodiscard]] bool from_string(std::string_view src, TYPE *dst, sn::error *err);
 * ```
 *
 * Both functions should return `true` on success. On failure, they should return `false`, and if `err` is not null,
 * write a description of the problem into `*err`. `*err` must not be touched on success. `err` should be passed through
 * when (de)serializing nested values.
 *
 * If you pass any tag types to this macro, they will be appended as additional arguments to the declared `std::string`
 * functions. Note that tags are always passed by value.
 *
 * A typical way to use this macro is:
 * - Invoke it in a header file for your type.
 * - Implement all functions in the cpp file.
 *
 * @param TYPE                          Type to generate `std::string` function declarations for.
 * @param ...                           Tags, if any.
 */
#define SN_DECLARE_STRING_FUNCTIONS(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool to_string(const TYPE &src, std::string *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool from_string(std::string_view src, TYPE *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__);

/**
 * Same as `SN_DECLARE_STRING_FUNCTIONS`, but declares friend functions. To be used inside a class definition.
 *
 * @see SN_DECLARE_STRING_FUNCTIONS
 */
#define SN_DECLARE_FRIEND_STRING_FUNCTIONS(TYPE, ... /* TAGS */) \
    friend bool to_string(const TYPE &src, std::string *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__); \
    friend bool from_string(std::string_view src, TYPE *dst, sn::error *err __VA_OPT__(,) __VA_ARGS__);
