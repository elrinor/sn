//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once
@INCLUDES@
#include "sn/core/preprocessor.h" // For __VA_OPT__.
@DECLS@
//
// A note on implementation.
//
// We used to have a single _SN_DECLARE_FUNCTIONS macro that did it all, and the macros in all *fwd.h headers were just
// invoking it. This was subsequently scrapped because we want the SN_DECLARE_* macros to be understandable for the
// people using them. People will come here, copy-paste the macro body, then paste it into their cpp file, and start
// implementing the functions.
//

/**
 * Generates `@TYPE@` function declarations for `TYPE` with tag types passed in varargs.
 *
 * The following declarations will be generated:
 * ```
 * [[nodiscard]] bool try_to_@LOWER@(const TYPE &src, @DST@) noexcept;
 * void to_@LOWER@(const TYPE &src, @DST@);
 * [[nodiscard]] bool try_from_@LOWER@(@SRC@, TYPE *dst) noexcept;
 * void from_@LOWER@(@SRC@, TYPE *dst);
 * ```
 *
 * If you pass any tag types to this macro, they will be appended as additional arguments to the declared `@TYPE@`
 * functions. Note that tags are always passed by value.
 *
 * A typical way to use this macro is:
 * - Invoke it in a header file for your type.
 * - Implement all functions in the cpp file.
 *
 * @param TYPE                          Type to generate `@TYPE@` function declarations for.
 * @param ...                           Tags, if any.
 */
#define SN_DECLARE_@UPPER@_FUNCTIONS(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool try_to_@LOWER@(const TYPE &src, @DST@ __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void to_@LOWER@(const TYPE &src, @DST@ __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool try_from_@LOWER@(@SRC@, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void from_@LOWER@(@SRC@, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * Same as `SN_DECLARE_@UPPER@_FUNCTIONS`, but declares friend functions. To be used inside a class definition.
 *
 * @see SN_DECLARE_@UPPER@_FUNCTIONS
 */
#define SN_DECLARE_FRIEND_@UPPER@_FUNCTIONS(TYPE, ... /* TAGS */) \
    friend bool try_to_@LOWER@(const TYPE &src, @DST@ __VA_OPT__(,) __VA_ARGS__) noexcept; \
    friend void to_@LOWER@(const TYPE &src, @DST@ __VA_OPT__(,) __VA_ARGS__); \
    friend bool try_from_@LOWER@(@SRC@, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    friend void from_@LOWER@(@SRC@, TYPE *dst __VA_OPT__(,) __VA_ARGS__);
