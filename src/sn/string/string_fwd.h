//
// This header is auto-generated using the headergen tool in /tools.
//
#pragma once

#include <string>
#include <string_view>

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
 * @internal
 *
 * Same as `SN_DECLARE_STRING_FUNCTIONS`, but `to_string` and `try_to_string` take `TYPE` by value. This can result in
 * better codegen on most architectures as the 1st arg can now be passed in registers.
 */
#define _SN_DECLARE_STRING_FUNCTIONS_BY_VALUE(TYPE, ... /* TAGS */) \
    [[nodiscard]] bool try_to_string(TYPE src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void to_string(TYPE src, std::string *dst __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool try_from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * @internal
 *
 * Unlike the `DECLARE_*` macros, this macro defines `std::string` functions for `TYPE` that just ignore the provided `TAG`,
 * effectively shifting the tag sequence by a single position to the left.
 */
#define _SN_DEFINE_INLINE_STRING_TAG_EATING_FUNCTIONS(TYPE, TAG) \
    template<class... Tags> \
    [[nodiscard]] inline bool try_to_string(const TYPE &src, std::string *dst, TAG tag, Tags... tags) noexcept { \
        return try_to_string(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void to_string(const TYPE &src, std::string *dst, TAG tag, Tags... tags) { \
        return to_string(src, dst, tags...); \
    } \
    template<class... Tags> \
    [[nodiscard]] inline bool try_from_string(std::string_view src, TYPE *dst, TAG tag, Tags... tags) noexcept { \
        return try_from_string(src, dst, tags...); \
    } \
    template<class... Tags> \
    inline void from_string(std::string_view src, TYPE *dst, TAG tag, Tags... tags) { \
        return from_string(src, dst, tags...); \
    }

/**
 * Generates `std::string` function declarations for `TYPE` with tag types passed in varargs.
 *
 * The following declarations will be generated:
 * ```
 * [[nodiscard]] bool try_to_string(const TYPE &src, std::string *dst) noexcept;
 * void to_string(const TYPE &src, std::string *dst);
 * [[nodiscard]] bool try_from_string(std::string_view src, TYPE *dst) noexcept;
 * void from_string(std::string_view src, TYPE *dst);
 * ```
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
    [[nodiscard]] bool try_to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__); \
    [[nodiscard]] bool try_from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    void from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);

/**
 * Same as `SN_DECLARE_STRING_FUNCTIONS`, but declares friend functions. To be used inside a class definition.
 *
 * @see SN_DECLARE_STRING_FUNCTIONS
 */
#define SN_DECLARE_FRIEND_STRING_FUNCTIONS(TYPE, ... /* TAGS */) \
    friend bool try_to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    friend void to_string(const TYPE &src, std::string *dst __VA_OPT__(,) __VA_ARGS__); \
    friend bool try_from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__) noexcept; \
    friend void from_string(std::string_view src, TYPE *dst __VA_OPT__(,) __VA_ARGS__);
